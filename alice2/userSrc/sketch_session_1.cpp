
// #define __MAIN__
#ifdef __MAIN__


#include <alice2.h>
#include <sketches/SketchRegistry.h>

using namespace alice2;
using namespace std;

int myInt;
float myFloat;
string myString;
int myInt2;

class Point{
    float x;
    float y;
    float z;
};

class Line{
    Point a;
    Point b;
};

vector<Vec3> points;
int num = 20;
int numRows = 10;
int numCols = 10;

Vec3 minBB(0,0,0);
Vec3 maxBB (10,10,0);


class Session_1_Sketch : public ISketch {
public:
    Session_1_Sketch() = default;
    ~Session_1_Sketch() = default;

    // Sketch lifecycle
    void setup() override {
        // Initialize your sketch here
        // This is called once when the sketch is loaded

        // The first line of code we did
        // printf("\nhello world\n");
        // cout << "hello world" << myInt << endl;

        // manual push back a lot of points
        // Vec3 pt(0, 0, 0);
        // Vec3 pt_1(0, 0, 0);
        // points.push_back(pt);
        // points.push_back(pt_1);

        // adding a row of points in x direction
        // for(int i = 0; i < num; i++)
        // {
        //     // in every iteration, we make a new point, add to the container "points"
        //     Vec3 myPoint(i, 0, 0);
        //     points.push_back(myPoint);
        // }

        // we use row major to create a 2d field points
        for(int i = 0; i < numCols; i++)
        {
            for(int j = 0; j < numRows; j++)
            {
                float x = minBB.x + i * (maxBB.x - minBB.x) / numRows;
                float y = minBB.y + j * (maxBB.y - minBB.y) / numCols;
                points.emplace_back(x, y, 0.0f);
            }
        }
    }

    void update(float deltaTime) override {
        // Update your sketch logic here
        // This is called every frame
        // deltaTime is the time elapsed since the last frame in seconds

        // printf("\nhello world\n");
    }

    void draw(Renderer& renderer, Camera& camera) override {
        // Draw your custom content here
        // This is called every frame after update()

        // make a point and draw
        // Vec3 pt(0, 0, 0);
        // Color col(1, 1, 1);
        // renderer.drawPoint(pt, col, 10);

        // Vec3 pt_1(1, 0, 0);
        // Color col_1(1, 0, 0);
        // renderer.drawPoint(pt_1, col_1, 10);
        // renderer.drawPoint(Vec3(0,0,0));

        // for(int i = 0; i < points.size(); i++)
        // {
        //     // iterate the container, draw the elements
        //     renderer.drawPoint(points[i], Color(1, 1, 1), 10);
        // }

        for(int i = 0; i < points.size(); i++)
        {
            renderer.drawPoint(points[i], Color(1, 1, 1), 10);
        }

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

