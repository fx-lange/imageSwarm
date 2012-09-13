uniform float focusDistance;
uniform float pointBrightness;
uniform float maxDistance;
uniform float sizeRange;
uniform float minSize;

const float PI = 3.14159265;

void main() {
// get the homogeneous 2d position
  gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;

  //sizeRange = 16-minSize;
  float size = (abs(gl_Position.z - focusDistance) / maxDistance) * sizeRange; 
  //TODO perhaps calc sizeRange / maxDistance on CPU
  size += minSize;
  
  gl_PointSize = size;

  gl_FrontColor = gl_Color;
  float radius = size / 2.;
  // divide the color alpha by the area
  float area =  PI * radius * radius;
  gl_FrontColor.a /= (area * (1.f-pointBrightness));
}