#include "utility.h"
#include <stdarg.h>

Vector3f generate_vector(float distance, float heading, float inclination)
{
    Vector3f vector;
    Matrix3f heading_transformation;
    Matrix3f inclination_transformation;

    vector << 1, 0, 0;

    // Rotate about z axis to apply heading
    heading_transformation << cos(heading), sin(heading), 0,
                              sin(heading), cos(heading), 0,
                              0         , 0         , 1;

    // Rotate about y axis to apply inclincation
    inclination_transformation << cos(inclination), 0, sin(inclination),
                              0         , 1,          0,
                              sin(inclination), 0, cos(inclination);

    vector = inclination_transformation*heading_transformation*vector;

    return -vector;
}

void debug(unsigned int mode, const char* str)
{
    if ((int)mode == 0 || (DEBUG_BOOL_ARR[(int)mode] && sizeof(str) < 250*sizeof(char)))
    {
        char buffer[250+6];
        sprintf(buffer, "%s: %s\n",DEBUG_STR_ARR[(int)mode],str);
        Serial.print(buffer);
    }
}

void debugf(unsigned int mode, const char *format, ...)
{
    if ((int)mode == 0 || (DEBUG_BOOL_ARR[(int)mode] && sizeof(format) < 250*sizeof(char)))
    {
        char buffer[250+6];
        va_list args;

        sprintf(buffer, "%s: ",DEBUG_STR_ARR[(int)mode]);
        Serial.print(buffer);

        vsprintf(buffer, format, args);
        Serial.print(buffer);

        Serial.print("\n");

    }
}

// Returns inclination and heading in a vector2d
Vector2d get_inclination_heading(Vector3f true_vec)
{
    Vector3f z_axis;
    Vector2d out;
    float dot_prod;
    float scaling;

    z_axis << 0,1,0;
    dot_prod = true_vec.dot(z_axis);
    scaling = z_axis.norm() * true_vec.norm();


    out[0] = RAD_TO_DEG * acos(scaling);

	out[1] = RAD_TO_DEG * atan2(true_vec(1), true_vec(0));

	return out;
}

