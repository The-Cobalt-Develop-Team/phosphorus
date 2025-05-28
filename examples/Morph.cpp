//
// Created by Renatus Madrigal on 5/28/2025.
//

// Visualize a graph. Minimal example showing how a default graph appears

#include <morph/Visual.h>
// Visual must be included before GraphVisual
#include <morph/GraphVisual.h>
#include <morph/vvec.h>

int main() {
  // Set up a morph::Visual 'scene environment'.
  morph::Visual v(1024, 768, "Made with morph::GraphVisual");
  // Create a new GraphVisual object with offset within the scene of 0,0,0
  auto gv = std::make_unique<morph::GraphVisual<double>>(
      morph::vec<float>({0, 0, 0}));
  // Boilerplate bindmodel function call - do this for every model you add to a
  // Visual
  v.bindmodel(gv);
  // Data for the x-axis. A vvec is like std::vector, but with built-in maths
  // methods
  morph::vvec<double> x;
  // This works like numpy's linspace() (the 3 args are "start", "end" and
  // "num"):
  x.linspace(-0.5, 0.8, 14);
  // Set a graph up of y = x^3
  gv->setdata(x, x.pow(3));
  // finalize() makes the GraphVisual compute the vertices of the OpenGL model
  gv->finalize();
  // Add the GraphVisual OpenGL model to the Visual scene (which takes ownership
  // of the unique_ptr)
  v.addVisualModel(gv);
  // Render the scene on the screen until user quits with 'Ctrl-q'
  v.keepOpen();
  v.saveImage("output.png"); // Save the rendered scene to an image file
  return 0;
}