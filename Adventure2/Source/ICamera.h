#pragma once
#include "pch.h"

class ICamera {

 public:
   ICamera(UINT width, UINT height, float fov = 70.f, float nearDist = 0.1f, float farDist = 10000.f)
       : m_width(width), m_height(height), m_fov(fov), m_nearDist(nearDist), m_farDist(farDist),
         m_view(DirectX::SimpleMath::Matrix::Identity), m_cameraPosition(DirectX::SimpleMath::Vector3::Zero) {
      m_projection = UpdateProjection();
   };
   ICamera(ICamera&&) = delete;
   ICamera(ICamera const&) = delete;

   virtual ~ICamera(){};
   // Update the view matrix based on gamepad input
   virtual void Update(float elapsedTime, DirectX::GamePad::State& pad) = 0;
   // Update the view matrix based on kb/mouse input
   virtual void Update(float elapsedTime, DirectX::Mouse& mouse, DirectX::Keyboard& kb) = 0;

   virtual void Resize(UINT width, UINT height) {
      m_width = width;
      m_height = height;
      m_projection = UpdateProjection();
   };

   inline DirectX::SimpleMath::Matrix GetView() const { return m_view; };
   inline DirectX::SimpleMath::Matrix GetProjection() const { return m_projection; };
   inline DirectX::SimpleMath::Vector3 GetPosition() const { return m_cameraPosition; };

 protected:
   DirectX::SimpleMath::Matrix m_view;
   DirectX::SimpleMath::Matrix m_projection;

   DirectX::SimpleMath::Vector3 m_cameraPosition;

   UINT m_width;
   UINT m_height;
   float m_fov;
   float m_nearDist;
   float m_farDist;

   DirectX::SimpleMath::Matrix UpdateProjection() {
      float aspectRatio = (m_height > 0.f) ? (float(m_width) / float(m_height)) : 1.f;
      return DirectX::SimpleMath::Matrix::CreatePerspectiveFieldOfView(m_fov, aspectRatio, m_nearDist, m_farDist);
   }
};