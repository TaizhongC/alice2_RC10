
#define __MAIN__
#ifdef __MAIN__


#include <alice2.h>
#include <sketches/SketchRegistry.h>

#include <computeGeom/scalarField.h>

using namespace alice2;
using namespace std;

Vec3 minBB(-10, -10, 0);
Vec3 maxBB(10, 10, 0);
ScalarField2D myField(minBB, maxBB);

bool d_points = false;
bool d_values = false;

class Session_2_Sketch : public ISketch {
public:
    Session_2_Sketch() = default;
    ~Session_2_Sketch() = default;

    // Sketch lifecycle
    void setup() override {
        myField.apply_scalar_rect(Vec3(0, 0, 0), Vec3(5, 3, 0), 0);
    }

    void update(float deltaTime) override {
    }

    void draw(Renderer& renderer, Camera& camera) override {
        if(d_points)
        {
            myField.draw_points(renderer, 2);
        }
        if(d_values)
        {
            myField.draw_values(renderer, 2);
        }

        renderer.setColor(Color(1, 0, 0));
        myField.drawIsocontours(renderer, 0);
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
            case 'p':
                d_points = !d_points;
                return true;
            case 'v':
                d_values = !d_values;
                return true;
            case 'c':
                // make a temporary field to host the circle
                ScalarField2D temp(minBB, maxBB);
                // add circle to the temp field
                temp.apply_scalar_circle(Vec3(2, 3, 0), 2);
                // use our original field to boolean union with the other,
                // it will update our original field
                myField.boolean_union(temp);
                // myField.boolean_subtract(temp);
                return true;
        }
        return false; // Not handled
    }
};

// Register the sketch with alice2 (both old and new systems)
ALICE2_REGISTER_SKETCH_AUTO(Session_2_Sketch)

#endif // __MAIN__

