#ifndef __UTILS__H__
#define __UTILS__H__

#include <Urho3D/Math/Vector3.h>
#include <Urho3D/Math/Color.h>

using namespace Urho3D;

namespace utils {
    Color GetBloodColor();
    extern float Distance(Vector3 from, Vector3 to);
}

#endif
