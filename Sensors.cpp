#include "sensors/Sensors.h"
#include "utils/NumericalMethods.h"

void InertialSensor::CalibrateLinear()
{
    RowVector<float,10> U;
    Matrix3f M;
    Vector3f n;
    float d;

    U = fit_ellipsoid(calibration_data);
    M << U[0], U[5], U[4], U[5], U[1], U[3], U[4], U[3], U[2];
    n << U[6], U[7], U[8];
    d = U[9];
    Vector<float, 12> mag_transformation = calculate_ellipsoid_transformation(M, n, d);

    calibration_matrix << mag_transformation[0], mag_transformation[1], mag_transformation[2], mag_transformation[3], mag_transformation[4], mag_transformation[5], mag_transformation[6], mag_transformation[7], mag_transformation[8];
    calibration_offset << mag_transformation[9], mag_transformation[10], mag_transformation[11];
}

Vector3f InertialSensor::GetCalibratedData()
{
    Vector3f reading;
    reading.setZero();
    for (int i=0;i<SAMPLES_PER_READING;i++)
    {
        reading += GetRawData();
    }
    reading = reading/SAMPLES_PER_READING;
    
    return calibration_matrix * (reading - calibration_offset);
}

bool InertialSensor::ColectCalibrationSample()
{
    calibration_data.col(calib_num) = GetRawData();
    calib_num++;
    if (calib_num == N_CALIB)
    {
        return 1;
    }
    return 0;
}

void InertialSensor::ResetCalibration()
{
    calib_num = 0;
    calibrated_data = VectorXf::Zero(3,N_CALIB);
}


bool SensorHandler::AlignLaser()
{
    laser->get_measurement();
    accelerometer->GetCalibratedData();
    magnetometer->GetCalibratedData();
    alignment_progress++;

    if (alignment_progress == N_ALIGNMENT)
    {
        // TODO
        // Execute alignment maths
        // Make some kind of warning if alignment appears to be low quality. Could be checked by checking angle between each of the spays for angle to te normal. Other method would be to do a test splay between the same two points and checking whether each shot is the inverse of the other
        return 1;
    }
    return 0;
}

void SensorHandler::ResetCalibration()
{
    magnetometer->ResetCalibration();
    accelerometer->ResetCalibration();
    alignment_progress = 0;
}

bool SensorHandler::CollectCalibrationData()
{
    if (magnetometer->ColectCalibrationSample() && accelerometer->ColectCalibrationSample())
    {
        return 1;
    }
    return 0;
}

void SensorHandler::CalibrateInertial()
{
    magnetometer->CalibrateLinear();
    accelerometer->CalibrateLinear();
    // TODO
    // Test and work on non-linear fitting with RBFs. Then add function void CalibrateNonLinear()
}