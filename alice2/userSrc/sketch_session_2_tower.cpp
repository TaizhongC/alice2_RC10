#define __MAIN__
#ifdef __MAIN__

#include <alice2.h>
#include <sketches/SketchRegistry.h>
#include <computeGeom/scalarField.h>
#include <computeGeom/ScalarField3D.h>
#include <objects/MeshObject.h>
#include <memory>
#include <cmath>
#include <algorithm>

using namespace alice2;
using std::vector;
using std::string;

// Shared bounds & resolution for all fields (must match for interpolate)
static Vec3 kMinBB(-10, -10, 0);
static Vec3 kMaxBB( 10,  10, 0);
static Vec3 kBottomRectHalfSize(5, 3, 0);
static constexpr float kDefaultFloorSpacing = 3.0f;
static constexpr float kTopCircleRadius = 1.0f;
static constexpr float kTopCircleOffsetX = 2.0f;
static constexpr float kTopCircleOffsetY = 1.5f;

class Session_2_TowerSketch : public ISketch {
public:
    // Endpoints
    ScalarField2D bottom_{kMinBB, kMaxBB};
    ScalarField2D top_   {kMinBB, kMaxBB};
    ScalarField2D scratch_{kMinBB, kMaxBB};

    // Generated slices (inclusive of endpoints)
    vector<ScalarField2D> slices_;

    // UI
    int   numLevels_ = 20;    // >= 2
    float iso_       = 0.0f;  // isocontour to draw
    bool  showPoints_ = false;
    bool  showValues_ = false;
    std::unique_ptr<SimpleUI> ui_;
    float towerHeight_ = (numLevels_ - 1) * kDefaultFloorSpacing;
    float towerHeightPrev_ = towerHeight_;
    float cornerRadius_ = 0.5f;
    float cornerRadiusPrev_ = cornerRadius_;
    std::shared_ptr<MeshData> isoMesh_;
    bool meshDirty_ = true;
    std::unique_ptr<MeshObject> isoMeshObject_;

    Session_2_TowerSketch() = default;
    ~Session_2_TowerSketch() = default;

    // --- lifecycle (ISketch) -------------------------------------------------
    void setup() override {
        buildEndpoints();     // rectangle (bottom) + circle (top)
        rebuildSlices();      // interpolate in-betweens
        ui_ = std::make_unique<SimpleUI>(input());
        ui_->addSlider("Tower Height", Vec2{10, 120}, 200.0f, 3.0f, 200.0f, towerHeight_);
        ui_->addSlider("Corner Radius", Vec2{10, 150}, 200.0f, 0.0f, 5.0f, cornerRadius_);
        isoMeshObject_ = std::make_unique<MeshObject>("TowerVolume");
        isoMeshObject_->setRenderMode(MeshRenderMode::NormalShaded);
        isoMeshObject_->setShowEdges(false);
        isoMeshObject_->setShowVertices(false);
    }

    void update(float /*deltaTime*/) override {
        if (ui_ && std::abs(cornerRadius_ - cornerRadiusPrev_) > 1e-4f) {
            cornerRadiusPrev_ = std::clamp(cornerRadius_, 0.0f, 5.0f);
            cornerRadius_ = cornerRadiusPrev_;
            buildEndpoints();
            rebuildSlices();
        }

        if (ui_ && std::abs(towerHeight_ - towerHeightPrev_) > 1e-4f) {
            towerHeightPrev_ = std::max(towerHeight_, 0.01f);
            meshDirty_ = true;
        }
    }

    void draw(Renderer& renderer, Camera& camera) override {
        if (showPoints_) bottom_.draw_points(renderer, 2);
        if (showValues_) bottom_.draw_values(renderer, 8);

        const float spacing = (slices_.size() <= 1)
            ? 0.0f
            : towerHeight_ / float(slices_.size() - 1);

        // Draw every slice's iso-contour in white
        for (int i = 0; i < (int)slices_.size(); ++i) {
            renderer.setColor(Color(1, 1, 1));

            renderer.pushMatrix();
            renderer.multMatrix(Mat4::translation(Vec3(0, 0, i * spacing)));
            slices_[i].drawIsocontours(renderer, iso_); // uses your field renderer
            renderer.popMatrix();
        }

        if (meshDirty_) {
            rebuildVolumeMesh();
        }
        if (isoMeshObject_ && isoMesh_) {
            isoMeshObject_->render(renderer, camera);
        }

        // HUD (Renderer::drawString expects (string, x, y))
        renderer.setColor(Color(1,1,1));
        renderer.drawString(string("Tower: bottom=rectangle, top=circle (linear interpolate)"), 10.f, 20.f);
        renderer.drawString(string("Levels [+/−]: ") + std::to_string(numLevels_), 10.f, 40.f);
        renderer.drawString(string("Iso     [[/]]: ") + std::to_string(iso_),      10.f, 60.f);
        renderer.drawString(string("P: points  V: values  R: reset endpoints"),    10.f, 80.f);
        if (ui_) {
            ui_->draw(renderer);
        }
    }

    void cleanup() override {
        // no-op
    }

    // --- input (ISketch) -----------------------------------------------------
    bool onKeyPress(unsigned char key, int /*x*/, int /*y*/) override {
        switch (key) {
        case '+': case '=':
            numLevels_ = std::min(64, numLevels_ + 1);
            rebuildSlices();
            return true;
        case '-': case '_':
            numLevels_ = std::max(2, numLevels_ - 1);
            rebuildSlices();
            return true;
        case '[':
            iso_ -= 0.1f;
            meshDirty_ = true;
            return true;
        case ']':
            iso_ += 0.1f;
            meshDirty_ = true;
            return true;
        case 'P': case 'p':
            showPoints_ = !showPoints_;
            return true;
        case 'V': case 'v':
            showValues_ = !showValues_;
            return true;
        case 'R': case 'r':
            buildEndpoints();
            rebuildSlices();
            return true;
        default:
            return false;
        }
    }

    bool onMousePress(int button, int state, int x, int y) override {
        if (ui_ && ui_->onMousePress(button, state, x, y)) {
            return true;
        }
        return false;
    }

    bool onMouseMove(int x, int y) override {
        if (ui_ && ui_->onMouseMove(x, y)) {
            return true;
        }
        return false;
    }

private:
    // Build the two endpoints on the SAME grid (required for interpolate)
    void buildEndpoints() {
        bottom_.clear_field();
        bottom_.apply_scalar_rect(Vec3(0, 0, 0), kBottomRectHalfSize, 0); // rectangle
        auto subtractCorner = [&](const Vec3& center) {
            scratch_.clear_field();
            scratch_.apply_scalar_circle(center, std::max(0.0f, cornerRadius_));
            bottom_.boolean_subtract(scratch_);
        };
        const float hx = kBottomRectHalfSize.x;
        const float hy = kBottomRectHalfSize.y;
        subtractCorner(Vec3( hx,  hy, 0));
        subtractCorner(Vec3(-hx,  hy, 0));
        subtractCorner(Vec3( hx, -hy, 0));
        subtractCorner(Vec3(-hx, -hy, 0));
        top_.clear_field();
        bool firstCircle = true;
        auto addTopCorner = [&](const Vec3& center) {
            scratch_.clear_field();
            scratch_.apply_scalar_circle(center, kTopCircleRadius);
            if (firstCircle) {
                top_ = scratch_;
                firstCircle = false;
            } else {
                top_.boolean_union(scratch_);
            }
        };
        addTopCorner(Vec3( kTopCircleOffsetX,  kTopCircleOffsetY, 0));
        addTopCorner(Vec3(-kTopCircleOffsetX,  kTopCircleOffsetY, 0));
        addTopCorner(Vec3( kTopCircleOffsetX, -kTopCircleOffsetY, 0));
        addTopCorner(Vec3(-kTopCircleOffsetX, -kTopCircleOffsetY, 0));
    }

    // Linear interpolation: f_i = lerp(bottom, top, t_i), i=0..N-1
    void rebuildSlices() {
        const int N = std::max(2, numLevels_);
        slices_.clear();
        slices_.reserve(N);
        for (int i = 0; i < N; ++i) {
            const float t = (N == 1) ? 0.f : float(i) / float(N - 1);
            ScalarField2D f = bottom_;   // copy grid + values
            f.interpolate(top_, t);      // per-cell blend toward top
            slices_.push_back(std::move(f));
        }
        meshDirty_ = true;
    }

    void rebuildVolumeMesh();
};

void Session_2_TowerSketch::rebuildVolumeMesh() {
    if (slices_.empty()) {
        isoMesh_.reset();
        if (isoMeshObject_) {
            isoMeshObject_->setMeshData(nullptr);
        }
        meshDirty_ = false;
        return;
    }

    const auto [resX, resY] = slices_.front().get_resolution();
    const int resZ = static_cast<int>(slices_.size());
    const size_t sliceSize = static_cast<size_t>(resX) * static_cast<size_t>(resY);
    std::vector<float> volumeValues;
    volumeValues.reserve(sliceSize * slices_.size());

    for (const auto& slice : slices_) {
        const auto& values = slice.get_values();
        volumeValues.insert(volumeValues.end(), values.begin(), values.end());
    }

    ScalarField3D volume(
        Vec3(kMinBB.x, kMinBB.y, 0.0f),
        Vec3(kMaxBB.x, kMaxBB.y, std::max(towerHeight_, 0.01f)),
        resX,
        resY,
        resZ);
    volume.set_values(volumeValues);
    isoMesh_ = volume.generate_mesh(iso_);
    if (isoMesh_ && isoMeshObject_) {
        isoMesh_->calculateNormals();
        isoMeshObject_->setMeshData(isoMesh_);
        isoMeshObject_->setRenderMode(MeshRenderMode::NormalShaded);
        isoMeshObject_->setNormalShadingColors(Color(0.1, 0.1, 0.1), Color(1, 1, 1));
    }
    meshDirty_ = false;
}

// Register with alice2
ALICE2_REGISTER_SKETCH_AUTO(Session_2_TowerSketch)

#endif // __MAIN__
