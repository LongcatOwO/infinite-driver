#include <infd/generator/meshbuilding/BuildingMeshBuilder.hpp>
#include "infd/generator/meshbuilding/Polygon.hpp"
#include "infd/util/Function.hpp"

namespace infd::generator::meshbuilding {

    using namespace Clipper2Lib;

    BuildingMeshBuilder::BuildingMeshBuilder(ChunkGenerator &generator, PathD& path, helpers::RandomType& random) :
        x(generator.x), y(generator.y), noise(generator.perlinNoise), originPath(path), random(random) {}

    auto BuildingMeshBuilder::findRadiusBounds(PathD &path) {
        struct Result {
            glm::vec2 radius = glm::vec2(0, std::numeric_limits<float>::max());
            PointD point;
        } result;

        for (PointD& p : path) {
            result.point = result.point + p;
        }
        result.point = PointD(result.point.x / path.size(), result.point.y / path.size());

        for (unsigned int i = 1; i < path.size(); i++) {
            float x = path[i].x-result.point.x;
            float y = path[i].y-result.point.y;

            float r = std::sqrt(x*x + y*y);

            if (r > result.radius[0]) result.radius[0] = r;
            if (r < result.radius[1]) result.radius[1] = r;
        }
        return result;
    }

    BuildingMeshBuilder::Result BuildingMeshBuilder::build() {
        PathD basePath = scalePath(originPath, scaleFactor);
        PathD path = shrinkPath(basePath, -ROAD_PADDING_WIDTH);

        if (path.empty()) {
            return {
                    mb.build(),
                    std::move(tri_mesh)
            };
        }

        scalePath(path, 1/scaleFactor);

        glm::vec2 heights = findHeightBounds(path);

        processHull(path, ROAD_HEIGHT+heights[0], (heights[1]-heights[0])-ROAD_HEIGHT);

        PathD buildingPath = shrinkPath(basePath, -ROAD_PADDING_WIDTH-BUILDING_PADDING_WIDTH);

        if (originPath.size() < MAX_KHRUSHCHEVKA) {
            generateKhrushchevka(buildingPath, heights[0]);
        } else {
            auto [radius, avg_pos] = findRadiusBounds(buildingPath);
            float radiusDiff = radius[0]/radius[1];
            if (radiusDiff < MAX_ECCENTRICITY) {
                generateSkyscraper(buildingPath, heights[0], radius[1], avg_pos);
            } else {
                generateKhrushchevka(buildingPath, heights[0]);
            }
        }

        return {
            mb.build(),
            std::move(tri_mesh)
        };
    }

    PathD& BuildingMeshBuilder::scalePath(PathD& path, float factor) {
        for (unsigned int i = 0; i < path.size(); i++) {
            path[i] = path[i]*factor;
        }

        return path;
    }

    PathD BuildingMeshBuilder::shrinkPath(PathD &path, float factor) {
        PathsD paths = InflatePaths(
                {path}, factor * scaleFactor, JoinType::Miter, EndType::Polygon
        );

        return paths.empty() ? PathD() : SimplifyPath(
                paths[0],
                0.1f
        );
    }

    void BuildingMeshBuilder::processRoof(PathD &path, float height) {
        Polygon polygon;
        for (PointD& point : path) {
            glm::vec2 p(point.x, point.y);
            polygon.addPoint(p);
        }

        std::vector<p2t::Triangle*> triangles = polygon.triangulate();
        for (p2t::Triangle* tri : triangles) {
            Triangle t = Triangle::convertTo(*tri, glm::vec3(height));
            t.addToMesh(mb, index);
            t.addToCollision(*tri_mesh);
        }
    }

    void BuildingMeshBuilder::processWalls(PathD &path, float height, float depth) {
        auto drawFunc = util::BindedMemberFunc(&BuildingMeshBuilder::drawCollidingTriangle, *this);

        for (unsigned int i = 0; i < path.size()-1; i++) {
            processVerticalWall(
                    glm::vec3(path[i].x, height, path[i].y),
                    glm::vec3(path[i+1].x, height, path[i+1].y),
                    depth,
                    drawFunc
            );
        }
        processVerticalWall(
                glm::vec3(path.back().x, height, path.back().y),
                glm::vec3(path.front().x, height, path.front().y),
                depth,
                drawFunc
        );
    }

    void BuildingMeshBuilder::drawTriangle(Triangle &tri) {
        tri.addToMesh(mb, index);
    }

    void BuildingMeshBuilder::drawCollidingTriangle(Triangle &tri) {
        tri.addToMesh(mb, index);
        tri.addToCollision(*tri_mesh);
    }

    glm::vec2 BuildingMeshBuilder::findHeightBounds(PathD &path) {
        glm::vec2 heights(ChunkGenerator::scaledPerlin(path.front().x + x, path.front().y + y, noise)*PERLIN_TERRAIN_FACTOR);
        for (unsigned int i = 1; i < path.size(); i++) {
            float height = ChunkGenerator::scaledPerlin(path[i].x + x, path[i].y + y, noise)*PERLIN_TERRAIN_FACTOR;
            if (height > heights[0]) heights[0] = height;
            if (height < heights[1]) heights[1] = height;
        }
        return heights;
    }

    void BuildingMeshBuilder::processHull(PathD &path, float height, float depth) {
        processWalls(path, height, depth);
        processRoof(path, height);
    }

    void BuildingMeshBuilder::generateKhrushchevka(PathD basis, float floorHeight) {
        PathD roof = BuildingMeshBuilder::shrinkPath(basis, -BUILDING_ROOF_INDENT);

        scalePath(basis, 1/scaleFactor);
        scalePath(roof, 1/scaleFactor);

        int floors = khrushchevkaDist(random);
        float buildingHeight = floors*BUILDING_STOREY_HEIGHT;

        processHull(basis, floorHeight+buildingHeight, -buildingHeight);
        processHull(roof, floorHeight+buildingHeight+BUILDING_ROOFCAP_HEIGHT, -BUILDING_ROOFCAP_HEIGHT);
    }

    PathD BuildingMeshBuilder::removeVertices(PathD &source, unsigned int count, unsigned int index) {
        PathD output;
        for (unsigned int i = 0; i < count; i++) {
            output.push_back(source[(i+index) % (source.size()-1)]);
        }
        return output;
    }

    void BuildingMeshBuilder::generateSkyscraper(PathD& basis, float floorHeight, float min_radius, PointD& center) {
        PathD hull = generatePolygon(min_radius/4, 8, center);

        PathsD children;
        for (PointD& origin : hull) {
            children.push_back(generatePolygon(min_radius/2 * skyscraperChildrenDist(random), 8, origin));
        }

        std::shuffle(children.begin(), children.end(), random);

        PathsD output = {hull};
        for (unsigned int i = 0; i < children.size(); i++) {
            if (probabilityDist(random) < SKYSCRAPER_LAYER_CHANCE) {
                output.push_back(unionPath(output.back(), children[i]));
            }
        }

        float height = 0.f;

        for (int i = output.size()-1; i >= 0; i--) {
            float layerHeight = BUILDING_STOREY_HEIGHT*skyscraperLayerDist(random);
            height += layerHeight;
            PathD layer = output[i];
            BuildingMeshBuilder::scalePath(layer, 1/scaleFactor);
            processHull(layer, floorHeight+height, -layerHeight);
        }
        {
            float layerHeight = BUILDING_STOREY_HEIGHT*skyscraperLayerDist(random)*2.f;
            height += layerHeight;

            PathD layer = BuildingMeshBuilder::shrinkPath(output.front(), -BUILDING_ROOF_INDENT);

            if (layer.empty()) {
                return;
            }

            BuildingMeshBuilder::scalePath(layer, 1/scaleFactor);
            processHull(layer, floorHeight+height, -layerHeight);
        }
    }

    PathD BuildingMeshBuilder::generatePolygon(float radius, unsigned int sides, const PointD& origin) {
        PathD outline;

        float theta = glm::two_pi<float>() / sides;

        for (int i = sides-1; i >= 0; i--) {
            outline.emplace_back(
                    origin.x+sin(i*theta)*radius,
                    origin.y+cos(i*theta)*radius
            );
        }

        return outline;
    }

    PathD BuildingMeshBuilder::unionPath(PathD &a, PathD &b) {
        PathsD a_ = {a};
        PathsD b_ = {b};

        return Union(a_, b_, FillRule::NonZero)[0];
    }
}