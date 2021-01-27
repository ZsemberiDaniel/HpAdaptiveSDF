// === common uniforms === 

// model transformations:
// uniform scaling
uniform float sScale;
// translation
uniform vec3 sTranslation;
/*const */float sOneOverScale = 1.0 / sScale;

// inner bounding box size
uniform vec3 modelScale;
// inner bounding box min corner in global coordinates
uniform vec3 modelTrans;

// outer bouinding box size
uniform vec3 sdfTexSize;
// outer bouinding box min corner in model coordinates
uniform vec3 sdfTexCorner;

// bouinding box border width
uniform vec3 sdfTexBorder;

// the usual proj*view matrix
uniform mat4 viewProj;

// procedural sdf parameters
uniform vec3 param1;
uniform vec3 param2;
