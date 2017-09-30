#ifndef CAMERA_H
#define CAMERA_H
#include <cv.h>

class Camera
{
public:
    Camera(int index=0);
    Camera(bool valid);
    virtual int getImage(IplImage* image);
    virtual int executeCmd(const char* name);
    virtual int getStrValue(const char* name, char* buffer);
    virtual int setStrValue(const char* name, const char* buffer);
    virtual int setIntValue(const char* name, int value);
    virtual int setFloatValue(const char* name, float value);
    virtual int getIntValue(const char* name, int& value);
    virtual int getFloatValue(const char* name, float& value);
    virtual	~Camera();
private:
    Camera* m_cam;
};

#endif // CAMERA_H
