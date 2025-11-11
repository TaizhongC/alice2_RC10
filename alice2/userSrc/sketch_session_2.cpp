
#define __MAIN__
#ifdef __MAIN__


#include <alice2.h>
#include <sketches/SketchRegistry.h>

using namespace alice2;
using namespace std;

class Session_2_Sketch : public ISketch {
public:
    Session_2_Sketch() = default;
    ~Session_2_Sketch() = default;

    // Sketch lifecycle
    void setup() override {
    }

    void update(float deltaTime) override {
    }

    void draw(Renderer& renderer, Camera& camera) override {
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
ALICE2_REGISTER_SKETCH_AUTO(Session_2_Sketch)

#endif // __MAIN__

