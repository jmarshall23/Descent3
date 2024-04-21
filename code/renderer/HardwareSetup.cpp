#include "RendererConfig.h"
#include <stdlib.h>
#include <string.h>
#include "3d.h"
#include "pserror.h"
#include "HardwareInternal.h"
#include "renderer.h"

// User-specified aspect ratio, stored as w/h
static float sAspect = 0.0f;

// initialize the 3d system
void g3_Init(void) { atexit(g3_Close); }

// close down the 3d system
void g3_Close(void) {}

// allows the program to specify an aspect ratio that overrides the renderer's
// The parameter is the w/h of the screen pixels
void g3_SetAspectRatio(float aspect) { sAspect = aspect; }

void g3_GetViewPortMatrix(float *viewMat) {
  // extract the viewport data from the renderer
  int viewportWidth, viewportHeight;
  int viewportX, viewportY;
  rend_GetProjectionScreenParameters(viewportX, viewportY, viewportWidth, viewportHeight);

  float viewportWidthOverTwo = ((float)viewportWidth) * 0.5f;
  float viewportHeightOverTwo = ((float)viewportHeight) * 0.5f;

  // setup the matrix
  memset(viewMat, 0, sizeof(float) * 16);
  viewMat[0] = viewportWidthOverTwo;
  viewMat[5] = -viewportHeightOverTwo;
  viewMat[12] = viewportWidthOverTwo + (float)viewportX;
  viewMat[13] = viewportHeightOverTwo + (float)viewportY;
  viewMat[10] = viewMat[15] = 1.0f;
}

void g3_GetProjectionMatrix(float zoom, float *projMat) {
  // Get window size
  int viewportWidth, viewportHeight;
  rend_GetProjectionParameters(&viewportWidth, &viewportHeight);

  // Compute aspect ratio for this ViewPort, assuming a 16:9 screen
  float aspectRatio = (float)viewportWidth / (float)viewportHeight;

  // Setup the matrix to all zeros
  memset(projMat, 0, sizeof(float) * 16);

  // Calculate 1/tan(fov / 2), fov expected in degrees
  float fov = 85.0f * zoom;
  float fovy = fov * 3.14159265f / 180.0f; // Convert degrees to radians
  float oOT = 1.0f / tan(fovy / 2.0f);

  // Fill in the matrix for a typical perspective projection
  projMat[0] = oOT / aspectRatio; // Scale x-axis by aspect ratio
  projMat[5] = oOT;               // y-axis scaling
  projMat[10] = 1.0f;            // Define z-axis for projection (assuming zFar >> zNear)
  projMat[11] = 1.0f;            // Indicate a projection matrix
  projMat[14] = -1.0f * (1.0f - 1e-7f);            // Near plane at -1 units
}

// start the frame
void g3_StartFrame(vector *view_pos, matrix *view_matrix, float zoom) {
  // initialize the viewport transform
  g3_GetViewPortMatrix((float *)gTransformViewPort);
  g3_GetProjectionMatrix(zoom, (float *)gTransformProjection);
  g3_GetModelViewMatrix(view_pos, view_matrix, (float *)gTransformModelView);
  g3_UpdateFullTransform();

  // get window size
  rend_GetProjectionParameters(&Window_width, &Window_height);

  // Set vars for projection
  Window_w2 = ((float)Window_width) * 0.5f;
  Window_h2 = ((float)Window_height) * 0.5f;

  // Compute aspect ratio for this window
  float screen_aspect = rend_GetAspectRatio();
  if (sAspect != 0.0f) {
    // check for user override
    screen_aspect = screen_aspect * 4.0f / 3.0f / sAspect;
  }
  float s = screen_aspect * (float)Window_height / (float)Window_width;

  if (s <= 0.0f) // JEFF: Should this have been 1.0f?
  {
    // scale x
    Matrix_scale.x = s;
    Matrix_scale.y = 1.0f;
  } else {
    Matrix_scale.y = 1.0f / s;
    Matrix_scale.x = 1.0f;
  }

  Matrix_scale.z = 1.0f;

  // Set the view variables
  View_position = *view_pos;
  View_zoom = zoom;
  Unscaled_matrix = *view_matrix;

  // Compute matrix scale for zoom and aspect ratio
  if (View_zoom <= 1.0f) {
    // zoom in by scaling z
    Matrix_scale.z = Matrix_scale.z * View_zoom;
  } else {
    // zoom out by scaling x and y
    float oOZ = 1.0f / View_zoom;
    Matrix_scale.x = Matrix_scale.x * oOZ;
    Matrix_scale.y = Matrix_scale.y * oOZ;
  }

  // Scale the matrix elements
  View_matrix.rvec = Unscaled_matrix.rvec * Matrix_scale.x;
  View_matrix.uvec = Unscaled_matrix.uvec * Matrix_scale.y;
  View_matrix.fvec = Unscaled_matrix.fvec * Matrix_scale.z;

  // Reset the list of free points
  InitFreePoints();

  // Reset the far clip plane
  g3_ResetFarClipZ();
}

// this doesn't do anything, but is here for completeness
void g3_EndFrame(void) {
  // make sure temp points are free
  CheckTempPoints();
}

// get the current view position
void g3_GetViewPosition(vector *vp) { *vp = View_position; }

void g3_GetViewMatrix(matrix *mat) { *mat = View_matrix; }

void g3_GetUnscaledMatrix(matrix *mat) { *mat = Unscaled_matrix; }

// Gets the matrix scale vector
void g3_GetMatrixScale(vector *matrix_scale) { *matrix_scale = Matrix_scale; }

