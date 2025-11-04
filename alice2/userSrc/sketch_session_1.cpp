
#define __MAIN__
#ifdef __MAIN__


#include <alice2.h>
#include <sketches/SketchRegistry.h>

using namespace alice2;

class Session_1_Sketch : public ISketch {
public:
    Session_1_Sketch() = default;
    ~Session_1_Sketch() = default;

    // Sketch lifecycle
    void setup() override {
        // Initialize your sketch here
        // This is called once when the sketch is loaded
    }

    void update(float deltaTime) override {
        // Update your sketch logic here
        // This is called every frame
        // deltaTime is the time elapsed since the last frame in seconds
    }

    void draw(Renderer& renderer, Camera& camera) override {
        // Draw your custom content here
        // This is called every frame after update()
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
};

// Register the sketch with alice2 (both old and new systems)
ALICE2_REGISTER_SKETCH_AUTO(Session_1_Sketch)

#endif // __MAIN__

