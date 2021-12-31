#include "pch.h"
#include "FreeLookCamera.h"

using namespace DirectX::SimpleMath;
using namespace DirectX;

const XMVECTORF32 START_POSITION = {0.f, 0.f, -10.f, 0.f};
constexpr float ROTATION_GAIN = 0.0004f;
constexpr float MOVEMENT_GAIN = 0.07f;

#define MY_PRINTF(...)                                                                                                 \
   {                                                                                                                   \
      char cad[512];                                                                                                   \
      sprintf_s(cad, __VA_ARGS__);                                                                                     \
      OutputDebugStringA(cad);                                                                                         \
   }

FreeLookCamera::FreeLookCamera(UINT width, UINT height)
    : ICamera(width, height), m_yaw(0.f), m_pitch(0.f), m_cameraPos(START_POSITION) {}

void FreeLookCamera::Update(float elapsedTime, const DirectX::GamePad::State& pad) {
   UNREFERENCED_PARAMETER(elapsedTime);
   UNREFERENCED_PARAMETER(pad);
}

void FreeLookCamera::Update(float elapsedTime, DirectX::Mouse& mouseDevice, DirectX::Keyboard& keyboard) {
   UNREFERENCED_PARAMETER(elapsedTime);
   auto mouse = mouseDevice.GetState();

   if (mouse.positionMode == Mouse::MODE_RELATIVE) {
      Vector3 delta = Vector3(float(mouse.x), float(mouse.y), 0.f) * ROTATION_GAIN;

      m_pitch -= delta.y;
      m_yaw -= delta.x;
   }

   mouseDevice.SetMode(mouse.leftButton ? Mouse::MODE_RELATIVE : Mouse::MODE_ABSOLUTE);

   auto kb = keyboard.GetState();

   if (kb.Home) {
      m_cameraPos = START_POSITION.v;
      m_pitch = m_yaw = 0;
   }

   Vector3 move = Vector3::Zero;

   if (kb.Up || kb.W) move.y += 1.f;

   if (kb.Down || kb.S) move.y -= 1.f;

   if (kb.Left || kb.A) move.x += 1.f;

   if (kb.Right || kb.D) move.x -= 1.f;

   if (kb.PageUp || kb.Space) move.z += 1.f;

   if (kb.PageDown || kb.X) move.z -= 1.f;

   Quaternion q = Quaternion::CreateFromYawPitchRoll(m_yaw, m_pitch, 0.f);

   move = Vector3::Transform(move, q);

   move *= MOVEMENT_GAIN;

   m_cameraPos += move;

   // limit pitch to straight up or straight down
   constexpr float limit = XM_PIDIV2 - 0.01f;
   m_pitch = std::max(-limit, m_pitch);
   m_pitch = std::min(+limit, m_pitch);

   // keep longitude in sane range by wrapping
   if (m_yaw > XM_PI) {
      m_yaw -= XM_2PI;
   } else if (m_yaw < -XM_PI) {
      m_yaw += XM_2PI;
   }

   float y = sinf(m_pitch);
   float r = cosf(m_pitch);
   float z = r * cosf(m_yaw);
   float x = r * sinf(m_yaw);

   XMVECTOR lookAt = m_cameraPos + Vector3(x, y, z);

   m_view = XMMatrixLookAtRH(m_cameraPos, lookAt, Vector3::Up);
}