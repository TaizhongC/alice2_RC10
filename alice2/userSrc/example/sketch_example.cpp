
// #define __MAIN__
#ifdef __MAIN__


#include <alice2.h>
#include <sketches/SketchRegistry.h>

using namespace alice2;

class ExampleSketch : public ISketch {
public:
    ExampleSketch() = default;
    ~ExampleSketch() = default;

    // Sketch information
    std::string getName() const override { return "Example Sketch"; }
    std::string getDescription() const override { return "Example sketch with UI"; }

    // Sketch lifecycle
    void setup() override {
        // Initialize your sketch here
        // This is called once when the sketch is loaded
        
        // Example: Set background color
        scene().setBackgroundColor(Color(0.1f, 0.1f, 0.1f));

        // Example: Enable grid
        scene().setShowGrid(true);
        scene().setGridSize(10.0f);
        scene().setGridDivisions(10);

        // Example: Enable axes
        scene().setShowAxes(true);
        scene().setAxesLength(2.0f);

        // --- Simple UI setup ---
        m_ui = std::make_unique<SimpleUI>(input());
        // slider
        m_ui->addSlider("Background", Vec2{10, 100}, 160.0f, 0.0f, 1.0f, m_backgroundCol);
        // toggles
        m_ui->addToggle("Compute", UIRect{10, 200, 140, 26}, m_compute);
    }

    void update(float deltaTime) override {
        // Update your sketch logic here
        // This is called every frame
        // deltaTime is the time elapsed since the last frame in seconds

        if(m_compute){

            m_compute = !m_compute;
        }
        PostUIUpdates();
    }

    void draw(Renderer& renderer, Camera& camera) override {
        // Draw your custom content here
        // This is called every frame after update()

        // 2D text rendering (screen overlay)
        renderer.setColor(Color(1.0f, 1.0f, 1.0f));
        renderer.drawString(getName(), 10, 30);
        renderer.drawString(getDescription(), 10, 50);

        renderer.setColor(Color(0.0f, 1.0f, 1.0f));
        renderer.drawString("FPS: " + std::to_string((Application::getInstance()->getFPS())), 10, 70);

        // Draw UI last
        if (m_ui) m_ui->draw(renderer);
    }

    void cleanup() override {
        // Clean up resources here
        // This is called when the sketch is unloaded
    }

    // Input handling (optional)
    bool onKeyPress(unsigned char key, int x, int y) override {
        // Handle keyboard input
        switch (key) {
            case 27: // ESC key
                // Example: Exit application
                return false; // Not handled - allow default exit
        }
        return false; // Not handled
    }

    bool onMousePress(int button, int state, int x, int y) override {
        if (m_ui && m_ui->onMousePress(button, state, x, y)) {
            return true; // UI consumed -> block default camera behavior
        }
        return false; // Not handled by UI
    }

    bool onMouseMove(int x, int y) override {
        if (m_ui && m_ui->onMouseMove(x, y)) {
            return true; // UI dragging
        }
        return false; // Not handled by UI
    }
private:
    void PostUIUpdates(){
        if (std::abs(m_backgroundCol - m_backgroundCol_prev) > 1e-4f){
            float c = std::clamp(m_backgroundCol, 0.0f, 1.0f);
            scene().setBackgroundColor(Color(c, c, c));
            m_backgroundCol_prev = m_backgroundCol;
        }
    }
    // UI state
    std::unique_ptr<SimpleUI> m_ui;
    float m_backgroundCol = 0.1f;
    float m_backgroundCol_prev = 0.1f;
    bool m_compute = false;
};

// Register the sketch with alice2 (both old and new systems)
ALICE2_REGISTER_SKETCH_AUTO(ExampleSketch)

#endif // __MAIN__

