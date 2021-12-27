#pragma once
#include "pch.h"
#include "ICamera.h"

class FreeLookCamera : public ICamera {

 public:
   FreeLookCamera(UINT width, UINT height);

   ~FreeLookCamera() {}

   void Update(float elapsedTime, const DirectX::GamePad::State& pad) override;
   void Update(float elapsedTime, DirectX::Mouse& mouse, DirectX::Keyboard& kb) override;

 protected:
   float m_yaw, m_pitch;
   DirectX::SimpleMath::Vector3 m_cameraPos;
   DirectX::SimpleMath::Quaternion m_rotation;
};