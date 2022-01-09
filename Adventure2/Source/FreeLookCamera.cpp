#include "pch.h"
#include "FreeLookCamera.h"
#include "Log.h"

using namespace DirectX::SimpleMath;
using namespace DirectX;

const XMVECTORF32 START_POSITION = {0.f, 0.f, -10.f, 0.f};
constexpr float ROTATION_GAIN = 0.0016f;
constexpr float MOVEMENT_GAIN = 0.07f;

#define MY_PRINTF(...)                                                                                                 \
   {                                                                                                                   \
      char cad[512];                                                                                                   \
      sprintf_s(cad, __VA_ARGS__);                                                                                     \
      OutputDebugStringA(cad);                                                                                         \
   }

FreeLookCamera::FreeLookCamera(UINT width, UINT height)
    : ICamera(width, height)
    , m_yaw(0.f)
    , m_pitch(0.f)
    , m_cameraPos(START_POSITION) {}

void FreeLookCamera::Update(float elapsedTime,
                            DirectX::Mouse& mouseDevice,
                            DirectX::Keyboard& keyboard,
                            DirectX::GamePad& pad) {
   UNREFERENCED_PARAMETER(elapsedTime);

   auto padState = pad.GetState(0);

   m_pitch -= padState.thumbSticks.rightY * 0.025f;
   m_yaw -= padState.thumbSticks.rightX * 0.025f;

   auto distance = Vector3::Zero;

   distance.z += padState.thumbSticks.leftY;
   distance.x -= padState.thumbSticks.leftX;

   auto mouse = mouseDevice.GetState();

   if (mouse.positionMode == Mouse::MODE_RELATIVE) {
      Vector3 delta = Vector3(float(mouse.x), float(mouse.y), 0.f) * ROTATION_GAIN;

      m_pitch -= delta.y;
      m_yaw -= delta.x;
   }

   mouseDevice.SetMode(mouse.rightButton ? Mouse::MODE_RELATIVE : Mouse::MODE_ABSOLUTE);

   auto kb = keyboard.GetState();

   if (kb.Home) {
      m_cameraPos = START_POSITION.v;
      m_pitch = m_yaw = 0;
   }

   if (kb.Up || kb.W) distance.z += 1.f;
   if (kb.Down || kb.S) distance.z -= 1.f;
   if (kb.Left || kb.A) distance.x += 1.f;
   if (kb.Right || kb.D) distance.x -= 1.f;
   if (kb.R || kb.PageUp) distance.y += 1.f;
   if (kb.F || kb.PageDown) distance.y -= 1.f;

   Quaternion q = Quaternion::CreateFromYawPitchRoll(m_yaw, -m_pitch, 0.f);
   auto transformed = Vector3::Transform(distance, q);

   m_cameraPos += Vector3(transformed);

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