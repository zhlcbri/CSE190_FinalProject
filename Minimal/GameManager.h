

	/************************************************************************************

	Authors     :   Bradley Austin Davis <bdavis@saintandreas.org>
	Copyright   :   Copyright Brad Davis. All Rights reserved.

	Licensed under the Apache License, Version 2.0 (the "License");
	you may not use this file except in compliance with the License.
	You may obtain a copy of the License at

	http://www.apache.org/licenses/LICENSE-2.0

	Unless required by applicable law or agreed to in writing, software
	distributed under the License is distributed on an "AS IS" BASIS,
	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
	See the License for the specific language governing permissions and
	limitations under the License.

	************************************************************************************/
#include <rpc/server.h>
#include <rpc/msgpack.hpp>
#include <rpc/this_handler.h>
#include <iostream>
#include <memory>
#include <exception>
#include <algorithm>
#include <math.h>
#include <time.h>
#include "Vector3d.hpp"
#include <rpc/rpc_error.h>
#include "mandelbrot.h"
#include <Windows.h>

#define __STDC_FORMAT_MACROS 1

#define FAIL(X) throw std::runtime_error(X)

	///////////////////////////////////////////////////////////////////////////////
	//
	// GLM is a C++ math library meant to mirror the syntax of GLSL 
	//

#include <glm/glm.hpp>
#include <glm/gtc/noise.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

	// Import the most commonly used types into the default namespace
	using glm::ivec3;
using glm::ivec2;
using glm::uvec2;
using glm::mat3;
using glm::mat4;
using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::quat;

///////////////////////////////////////////////////////////////////////////////
//
// GLEW gives cross platform access to OpenGL 3.x+ functionality.  
//

#include <GL/glew.h>

////// self defined stuff below
#include "GameManager.h"

using namespace std;
using namespace glm;

GameManager * gameManager;
rpc::server srv(7897);
////////////


//======= move to GameManager ==========
vec3 hand_track = vec3(1.0f);

// hand position and orientation
vec3 handPos[2];
mat4 handRotation[2];
quat handQuaternion[2]; // temp

						// button states
bool button_X = false;
bool button_Y = false;
bool button_A = false;
bool button_B = false;
bool isPressed = false;

bool doOnce = true; // temp

bool checkFramebufferStatus(GLenum target = GL_FRAMEBUFFER) {
	GLuint status = glCheckFramebufferStatus(target);
	switch (status) {
	case GL_FRAMEBUFFER_COMPLETE:
		return true;
		break;

	case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
		std::cerr << "framebuffer incomplete attachment" << std::endl;
		break;

	case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
		std::cerr << "framebuffer missing attachment" << std::endl;
		break;

	case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
		std::cerr << "framebuffer incomplete draw buffer" << std::endl;
		break;

	case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
		std::cerr << "framebuffer incomplete read buffer" << std::endl;
		break;

	case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
		std::cerr << "framebuffer incomplete multisample" << std::endl;
		break;

	case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
		std::cerr << "framebuffer incomplete layer targets" << std::endl;
		break;

	case GL_FRAMEBUFFER_UNSUPPORTED:
		std::cerr << "framebuffer unsupported internal format or image" << std::endl;
		break;

	default:
		std::cerr << "other framebuffer error" << std::endl;
		break;
	}

	return false;
}

bool checkGlError() {
	GLenum error = glGetError();
	if (!error) {
		return false;
	}
	else {
		switch (error) {
		case GL_INVALID_ENUM:
			std::cerr << ": An unacceptable value is specified for an enumerated argument.The offending command is ignored and has no other side effect than to set the error flag.";
			break;
		case GL_INVALID_VALUE:
			std::cerr << ": A numeric argument is out of range.The offending command is ignored and has no other side effect than to set the error flag";
			break;
		case GL_INVALID_OPERATION:
			std::cerr << ": The specified operation is not allowed in the current state.The offending command is ignored and has no other side effect than to set the error flag..";
			break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			std::cerr << ": The framebuffer object is not complete.The offending command is ignored and has no other side effect than to set the error flag.";
			break;
		case GL_OUT_OF_MEMORY:
			std::cerr << ": There is not enough memory left to execute the command.The state of the GL is undefined, except for the state of the error flags, after this error is recorded.";
			break;
		case GL_STACK_UNDERFLOW:
			std::cerr << ": An attempt has been made to perform an operation that would cause an internal stack to underflow.";
			break;
		case GL_STACK_OVERFLOW:
			std::cerr << ": An attempt has been made to perform an operation that would cause an internal stack to overflow.";
			break;
		}
		return true;
	}
}

void glDebugCallbackHandler(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *msg, GLvoid* data) {
	OutputDebugStringA(msg);
	std::cout << "debug call: " << msg << std::endl;
}

//////////////////////////////////////////////////////////////////////
//
// GLFW provides cross platform window creation
//

#include <GLFW/glfw3.h>

namespace glfw {
	inline GLFWwindow * createWindow(const uvec2 & size, const ivec2 & position = ivec2(INT_MIN)) {
		GLFWwindow * window = glfwCreateWindow(size.x, size.y, "glfw", nullptr, nullptr);
		if (!window) {
			FAIL("Unable to create rendering window");
		}
		if ((position.x > INT_MIN) && (position.y > INT_MIN)) {
			glfwSetWindowPos(window, position.x, position.y);
		}
		return window;
	}
}

// A class to encapsulate using GLFW to handle input and render a scene
class GlfwApp {

protected:
	uvec2 windowSize;
	ivec2 windowPosition;
	GLFWwindow * window{ nullptr };
	unsigned int frame{ 0 };

public:
	GlfwApp() {
		// Initialize the GLFW system for creating and positioning windows
		if (!glfwInit()) {
			FAIL("Failed to initialize GLFW");
		}
		glfwSetErrorCallback(ErrorCallback);
	}

	virtual ~GlfwApp() {
		if (nullptr != window) {
			glfwDestroyWindow(window);
		}
		glfwTerminate();
	}

	virtual int run() {
		preCreate();

		window = createRenderingTarget(windowSize, windowPosition);

		if (!window) {
			std::cout << "Unable to create OpenGL window" << std::endl;
			return -1;
		}

		postCreate();

		initGl();

		while (!glfwWindowShouldClose(window)) {
			++frame;
			glfwPollEvents();
			update();
			draw();
			finishFrame();
		}

		shutdownGl();

		return 0;
	}


protected:
	virtual GLFWwindow * createRenderingTarget(uvec2 & size, ivec2 & pos) = 0;

	virtual void draw() = 0;

	void preCreate() {
		glfwWindowHint(GLFW_DEPTH_BITS, 16);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
	}


	void postCreate() {
		glfwSetWindowUserPointer(window, this);
		glfwSetKeyCallback(window, KeyCallback);
		glfwSetMouseButtonCallback(window, MouseButtonCallback);
		glfwMakeContextCurrent(window);

		// Initialize the OpenGL bindings
		// For some reason we have to set this experminetal flag to properly
		// init GLEW if we use a core context.
		glewExperimental = GL_TRUE;
		if (0 != glewInit()) {
			FAIL("Failed to initialize GLEW");
		}
		glGetError();

		if (GLEW_KHR_debug) {
			GLint v;
			glGetIntegerv(GL_CONTEXT_FLAGS, &v);
			if (v & GL_CONTEXT_FLAG_DEBUG_BIT) {
				//glDebugMessageCallback(glDebugCallbackHandler, this);
			}
		}
	}

	virtual void initGl() {
	}

	virtual void shutdownGl() {
	}

	virtual void finishFrame() {
		glfwSwapBuffers(window);
	}

	virtual void destroyWindow() {
		glfwSetKeyCallback(window, nullptr);
		glfwSetMouseButtonCallback(window, nullptr);
		glfwDestroyWindow(window);
	}

	virtual void onKey(int key, int scancode, int action, int mods) {
		if (GLFW_PRESS != action) {
			return;
		}

		switch (key) {
		case GLFW_KEY_ESCAPE:
			glfwSetWindowShouldClose(window, 1);
			return;
		}
	}

	virtual void update() {}

	virtual void onMouseButton(int button, int action, int mods) {}

protected:
	virtual void viewport(const ivec2 & pos, const uvec2 & size) {
		glViewport(pos.x, pos.y, size.x, size.y);
	}

private:

	static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
		GlfwApp * instance = (GlfwApp *)glfwGetWindowUserPointer(window);
		instance->onKey(key, scancode, action, mods);
	}

	static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
		GlfwApp * instance = (GlfwApp *)glfwGetWindowUserPointer(window);
		instance->onMouseButton(button, action, mods);
	}

	static void ErrorCallback(int error, const char* description) {
		FAIL(description);
	}
};

//////////////////////////////////////////////////////////////////////
//
// The Oculus VR C API provides access to information about the HMD
//

#include <OVR_CAPI.h>
#include <OVR_CAPI_GL.h>

namespace ovr {

	// Convenience method for looping over each eye with a lambda
	template <typename Function>
	inline void for_each_eye(Function function) {
		for (ovrEyeType eye = ovrEyeType::ovrEye_Left;
			eye < ovrEyeType::ovrEye_Count;
			eye = static_cast<ovrEyeType>(eye + 1)) {
			function(eye);
		}
	}

	inline mat4 toGlm(const ovrMatrix4f & om) {
		return glm::transpose(glm::make_mat4(&om.M[0][0]));
	}

	inline mat4 toGlm(const ovrFovPort & fovport, float nearPlane = 0.01f, float farPlane = 10000.0f) {
		return toGlm(ovrMatrix4f_Projection(fovport, nearPlane, farPlane, true));
	}

	inline vec3 toGlm(const ovrVector3f & ov) {
		return glm::make_vec3(&ov.x);
	}

	inline vec2 toGlm(const ovrVector2f & ov) {
		return glm::make_vec2(&ov.x);
	}

	inline uvec2 toGlm(const ovrSizei & ov) {
		return uvec2(ov.w, ov.h);
	}

	inline quat toGlm(const ovrQuatf & oq) {
		return glm::make_quat(&oq.x);
	}

	inline mat4 toGlm(const ovrPosef & op) {
		mat4 orientation = glm::mat4_cast(toGlm(op.Orientation));
		mat4 translation = glm::translate(mat4(), ovr::toGlm(op.Position));
		return translation * orientation;
	}

	inline ovrMatrix4f fromGlm(const mat4 & m) {
		ovrMatrix4f result;
		mat4 transposed(glm::transpose(m));
		memcpy(result.M, &(transposed[0][0]), sizeof(float) * 16);
		return result;
	}

	inline ovrVector3f fromGlm(const vec3 & v) {
		ovrVector3f result;
		result.x = v.x;
		result.y = v.y;
		result.z = v.z;
		return result;
	}

	inline ovrVector2f fromGlm(const vec2 & v) {
		ovrVector2f result;
		result.x = v.x;
		result.y = v.y;
		return result;
	}

	inline ovrSizei fromGlm(const uvec2 & v) {
		ovrSizei result;
		result.w = v.x;
		result.h = v.y;
		return result;
	}

	inline ovrQuatf fromGlm(const quat & q) {
		ovrQuatf result;
		result.x = q.x;
		result.y = q.y;
		result.z = q.z;
		result.w = q.w;
		return result;
	}
}

class RiftManagerApp {
protected:
	ovrSession _session;
	ovrHmdDesc _hmdDesc;
	ovrGraphicsLuid _luid;

public:
	RiftManagerApp() {
		if (!OVR_SUCCESS(ovr_Create(&_session, &_luid))) {
			FAIL("Unable to create HMD session");
		}

		_hmdDesc = ovr_GetHmdDesc(_session);
	}

	~RiftManagerApp() {
		ovr_Destroy(_session);
		_session = nullptr;
	}
};

class RiftApp : public GlfwApp, public RiftManagerApp {
public:

private:
	GLuint _fbo{ 0 };
	GLuint _depthBuffer{ 0 };
	ovrTextureSwapChain _eyeTexture;

	GLuint _mirrorFbo{ 0 };
	ovrMirrorTexture _mirrorTexture;

	ovrEyeRenderDesc _eyeRenderDescs[2];

	mat4 _eyeProjections[2];

	ovrLayerEyeFov _sceneLayer;
	ovrViewScaleDesc _viewScaleDesc;

	uvec2 _renderTargetSize;
	uvec2 _mirrorSize;

public:

	RiftApp() {
		using namespace ovr;
		_viewScaleDesc.HmdSpaceToWorldScaleInMeters = 1.0f;

		memset(&_sceneLayer, 0, sizeof(ovrLayerEyeFov));
		_sceneLayer.Header.Type = ovrLayerType_EyeFov;
		_sceneLayer.Header.Flags = ovrLayerFlag_TextureOriginAtBottomLeft;

		ovr::for_each_eye([&](ovrEyeType eye) {
			ovrEyeRenderDesc& erd = _eyeRenderDescs[eye] = ovr_GetRenderDesc(_session, eye, _hmdDesc.DefaultEyeFov[eye]);
			ovrMatrix4f ovrPerspectiveProjection =
				ovrMatrix4f_Projection(erd.Fov, 0.01f, 1000.0f, ovrProjection_ClipRangeOpenGL);
			_eyeProjections[eye] = ovr::toGlm(ovrPerspectiveProjection);
			_viewScaleDesc.HmdToEyePose[eye] = erd.HmdToEyePose;

			ovrFovPort & fov = _sceneLayer.Fov[eye] = _eyeRenderDescs[eye].Fov;
			auto eyeSize = ovr_GetFovTextureSize(_session, eye, fov, 1.0f);
			_sceneLayer.Viewport[eye].Size = eyeSize;
			_sceneLayer.Viewport[eye].Pos = { (int)_renderTargetSize.x, 0 };

			_renderTargetSize.y = std::max(_renderTargetSize.y, (uint32_t)eyeSize.h);
			_renderTargetSize.x += eyeSize.w;
		});
		// Make the on screen window 1/4 the resolution of the render target
		_mirrorSize = _renderTargetSize;
		_mirrorSize /= 2;
	}

protected:
	GLFWwindow * createRenderingTarget(uvec2 & outSize, ivec2 & outPosition) override {
		return glfw::createWindow(_mirrorSize);
	}

	void initGl() override {
		GlfwApp::initGl();

		// Disable the v-sync for buffer swap
		glfwSwapInterval(0);

		ovrTextureSwapChainDesc desc = {};
		desc.Type = ovrTexture_2D;
		desc.ArraySize = 1;
		desc.Width = _renderTargetSize.x;
		desc.Height = _renderTargetSize.y;
		desc.MipLevels = 1;
		desc.Format = OVR_FORMAT_R8G8B8A8_UNORM_SRGB;
		desc.SampleCount = 1;
		desc.StaticImage = ovrFalse;
		ovrResult result = ovr_CreateTextureSwapChainGL(_session, &desc, &_eyeTexture);
		_sceneLayer.ColorTexture[0] = _eyeTexture;
		if (!OVR_SUCCESS(result)) {
			FAIL("Failed to create swap textures");
		}

		int length = 0;
		result = ovr_GetTextureSwapChainLength(_session, _eyeTexture, &length);
		if (!OVR_SUCCESS(result) || !length) {
			FAIL("Unable to count swap chain textures");
		}
		for (int i = 0; i < length; ++i) {
			GLuint chainTexId;
			ovr_GetTextureSwapChainBufferGL(_session, _eyeTexture, i, &chainTexId);
			glBindTexture(GL_TEXTURE_2D, chainTexId);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		}
		glBindTexture(GL_TEXTURE_2D, 0);

		// Set up the framebuffer object
		glGenFramebuffers(1, &_fbo);
		glGenRenderbuffers(1, &_depthBuffer);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _fbo);
		glBindRenderbuffer(GL_RENDERBUFFER, _depthBuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, _renderTargetSize.x, _renderTargetSize.y);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _depthBuffer);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

		ovrMirrorTextureDesc mirrorDesc;
		memset(&mirrorDesc, 0, sizeof(mirrorDesc));
		mirrorDesc.Format = OVR_FORMAT_R8G8B8A8_UNORM_SRGB;
		mirrorDesc.Width = _mirrorSize.x;
		mirrorDesc.Height = _mirrorSize.y;
		if (!OVR_SUCCESS(ovr_CreateMirrorTextureGL(_session, &mirrorDesc, &_mirrorTexture))) {
			FAIL("Could not create mirror texture");
		}
		glGenFramebuffers(1, &_mirrorFbo);
	}

	void update() final override
	{
		// reset booleans
		button_X = false;

		ovrInputState inputState;
		if (OVR_SUCCESS(ovr_GetInputState(_session, ovrControllerType_Touch, &inputState)))
		{
			// Button controls
			if (!inputState.Buttons) {
				isPressed = false;
			}

			// when 'X' is pressed
			if ((inputState.Buttons & ovrButton_X) && !isPressed) {
				isPressed = true;
				button_X = true;
			}
		}
	}

	void onKey(int key, int scancode, int action, int mods) override {
		if (GLFW_PRESS == action) switch (key) {
		case GLFW_KEY_R:
			ovr_RecenterTrackingOrigin(_session);
			return;
		}

		GlfwApp::onKey(key, scancode, action, mods);
	}

	void draw() final override {

		// Query Touch controllers. Query their parameters:
		double displayMidpointSeconds = ovr_GetPredictedDisplayTime(_session, 0);
		ovrTrackingState trackState = ovr_GetTrackingState(_session, displayMidpointSeconds, ovrTrue);

		// Process controller status. Useful to know if controller is being used at all, and if the cameras can see it. 
		unsigned int handStatus[2];
		handStatus[0] = trackState.HandStatusFlags[0];
		handStatus[1] = trackState.HandStatusFlags[1];

		// Process controller position and orientation:
		ovrPosef handPoses[2];  // These are position and orientation in meters in room coordinates, relative to tracking origin. Right-handed cartesian coordinates.
								// ovrQuatf     Orientation;
								// ovrVector3f  Position;
		handPoses[0] = trackState.HandPoses[0].ThePose;
		handPoses[1] = trackState.HandPoses[1].ThePose;
		ovrVector3f handPosition[2];
		handPosition[0] = handPoses[0].Position;
		handPosition[1] = handPoses[1].Position;

		// make hand position and orientation global
		handPos[0] = ovr::toGlm(handPosition[0]);
		handPos[1] = ovr::toGlm(handPosition[1]);

		handRotation[0] = toMat4(ovr::toGlm(handPoses[0].Orientation));
		handRotation[1] = toMat4(ovr::toGlm(handPoses[1].Orientation));

		// hand rotation in quaternion; used to compute hand forward vector
		handQuaternion[0] = quat_cast(handRotation[0]);
		handQuaternion[1] = quat_cast(handRotation[1]);

		ovrPosef eyePoses[2];
		ovr_GetEyePoses(_session, frame, true, _viewScaleDesc.HmdToEyePose, eyePoses, &_sceneLayer.SensorSampleTime);

		int curIndex;
		ovr_GetTextureSwapChainCurrentIndex(_session, _eyeTexture, &curIndex);
		GLuint curTexId;
		ovr_GetTextureSwapChainBufferGL(_session, _eyeTexture, curIndex, &curTexId);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _fbo);
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, curTexId, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		ovr::for_each_eye([&](ovrEyeType eye) {
			const auto& vp = _sceneLayer.Viewport[eye];
			glViewport(vp.Pos.x, vp.Pos.y, vp.Size.w, vp.Size.h);
			_sceneLayer.RenderPose[eye] = eyePoses[eye];
			renderScene(_eyeProjections[eye], ovr::toGlm(eyePoses[eye]));
		});
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		ovr_CommitTextureSwapChain(_session, _eyeTexture);
		ovrLayerHeader* headerList = &_sceneLayer.Header;
		ovr_SubmitFrame(_session, frame, &_viewScaleDesc, &headerList, 1);

		GLuint mirrorTextureId;
		ovr_GetMirrorTextureBufferGL(_session, _mirrorTexture, &mirrorTextureId);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, _mirrorFbo);
		glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mirrorTextureId, 0);
		glBlitFramebuffer(0, 0, _mirrorSize.x, _mirrorSize.y, 0, _mirrorSize.y, _mirrorSize.x, 0, GL_COLOR_BUFFER_BIT, GL_NEAREST);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	}

	virtual void renderScene(const glm::mat4 & projection, const glm::mat4 & headPose) = 0;
};


// An example application that renders a simple cube
class ExampleApp : public RiftApp {
	//std::shared_ptr<ColorCubeScene> cubeScene;

public:
	ExampleApp() { }

protected:
	void initGl() override {
		RiftApp::initGl();
		glClearColor(0.2f, 0.2f, 0.2f, 0.0f);
		glEnable(GL_DEPTH_TEST);
		ovr_RecenterTrackingOrigin(_session);

		gameManager = new GameManager();
		gameManager->playSound();
	}

	void shutdownGl() override {
		gameManager->closeSound();
	}

	vec3 getForwardVector(quat q) {
		float x = q.x;
		float y = q.y;
		float z = q.z;
		float w = q.w;

		const float x2 = 2.0f * x;
		const float y2 = 2.0f * y;
		const float z2 = 2.0f * z;
		const float x2w = x2 * w;
		const float y2w = y2 * w;
		const float x2x = x2 * x;
		const float z2x = z2 * x;
		const float y2y = y2 * y;
		const float z2y = z2 * y;

		return vec3(z2x + y2w, z2y - x2w, 1.0f - (x2x + y2y));
	}

	void renderScene(const glm::mat4 & projection, const glm::mat4 & headPose) override {
		// update cube spinning angle
		angle_r += deg;
		if (angle_r > 360.0f || angle_r < -360.0f) angle_r = 0.0f;

		// gogo algorithm
		vec3 handForward = getForwardVector(handQuaternion[0]);
		vec3 gogoPos = gameManager->gogoHand(handPos[0], inverse(headPose)[3], -(handForward));

		// =========== left hand ==============
		mat4 T_hand = translate(mat4(1.0f), gogoPos);
		//mat4 T_hand = translate(mat4(1.0f), handPos[0]);
		mat4 S_hand = scale(mat4(1.0f), vec3(0.005, 0.005, 0.005));
		mat4 R_hand = handRotation[0];
		mat4 M_hand = T_hand * R_hand * S_hand;
		gameManager->renderHand(projection, inverse(headPose), M_hand);

		//============== skybox ===============
		mat4 M_skybox = scale(mat4(1.0f), vec3(325.0f, 325.0f, 325.0f));
		gameManager->renderSkybox(projection, inverse(headPose), M_skybox);

		// ============ cubes ==============
		mat4 T_cubeX = translate(mat4(1.0f), cube_track);
		mat4 S_cubeX = scale(mat4(1.0f), vec3(0.1f, 0.1f, 0.1f));
		mat4 R_cubeX = rotate(mat4(1.0f), angle_r / 180.0f*pi<float>(), vec3(0.0, 1.0, 0.0));
		mat4 M_cubeX = T_cubeX * R_cubeX * S_cubeX;

		//=========== particles =============
		bool beatHit = (gameManager->colliding(vec3(M_hand[3]), vec3(M_cubeX[3]), 0.2f) && button_X);

		if (doOnce && !beatHit) {
			gameManager->dropCubes(T_cubeX, S_cubeX, projection, inverse(headPose));
			//gameManager->rainCubes(projection, inverse(headPose));
		}
		else {
			doOnce = false;
			gameManager->renderParticles(projection, inverse(headPose), M_cubeX);
		}
		hand_track = handPos[0];
		cube_track.y -= 0.001;
	}
};

#include <rpc/server.h>
#include <rpc/this_handler.h>
#include <rpc/msgpack.hpp>
double divide(double a, double b) {
	if (b == 0.0) {
		rpc::this_handler().respond_error(
			std::make_tuple(1, "Division by zero"));
	}
	return a / b;
}

struct subtractor {
	double operator()(double a, double b) {
		return a - b;
	}
};

struct multiplier {
	double multiply(double a, double b) {
		return a * b;
	}
};
#include <rpc/msgpack.hpp>
// Execute our example class


int main(int argc, char ** argv) {


	pixel_data data1;
	pixel_data data2;
	data1.push_back({ 0,0,0 });//head position 0
	data1.push_back({ 0,0,0 });//lefthand position 0
	data1.push_back({ 0,0,0 });//right hand pos
	data1.push_back({ 0,0,0 });//cube1 pos
	data1.push_back({ 0,0,0 });//cube2 pos
	data1.push_back({ 0,0,0 });//cube3 pos
	data1.push_back({ 0,0,0 });//cube4 pos
	data1.push_back({ 0,0,0 });//score value


	data2.push_back({ 0,0,0 });//head position 0
	data2.push_back({ 0,0,0 });//lefthand position 0
	data2.push_back({ 0,0,0 });//right hand pos
	data2.push_back({ 0,0,0 });//cube1 pos
	data2.push_back({ 0,0,0 });//cube2 pos
	data2.push_back({ 0,0,0 });//cube3 pos
	data2.push_back({ 0,0,0 });//cube4 pos
	data2.push_back({ 0,0,0 });//score value
	int visit1 = 0, visit2 = 0;
	try {
		srv.bind("get_mandelbrot1", [&](double a, double b, double c, double d, double e, double f,
			double g, double h, double i, double j, double k, double l,
			double m, double n, double o, double p, double q, double r,
			double s, double t, double u, double v, double w, double x
			) {

			data1[0] = pixel({ a,b,c });//head
			data1[1] = pixel({ d, e, f });
			data1[2] = pixel({ g, h, i });
			data1[3] = pixel({ j,k,l });//head
			data1[4] = pixel({ m, n, o });
			data1[5] = pixel({ p, q, r });
			data1[6] = pixel({ s,t,u });//head
			data1[7] = pixel({ v, v, v });//score


			return data2;
		});
	}
	catch (rpc::rpc_error &e) {
		std::cout << std::endl << e.what() << std::endl;
	}
	try {
		srv.bind("get_mandelbrot2", [&](double a, double b, double c, double d, double e, double f,
			double g, double h, double i, double j, double k, double l,
			double m, double n, double o, double p, double q, double r,
			double s, double t, double u, double v, double w, double x
			) {

			data2[0] = pixel({ a,b,c });//head
			data2[1] = pixel({ d, e, f });
			data2[2] = pixel({ g, h, i });
			data2[3] = pixel({ j,k,l });//head
			data2[4] = pixel({ m, n, o });
			data2[5] = pixel({ p, q, r });
			data2[6] = pixel({ s,t,u });//head
			data2[7] = pixel({ v, w, x });


			return data1;
		});
	}
	catch (rpc::rpc_error &e) {
		std::cout << std::endl << e.what() << std::endl;
	}

	srv.run();



	int result = -1;
	try {
		if (!OVR_SUCCESS(ovr_Initialize(nullptr))) {
			FAIL("Failed to initialize the Oculus SDK");
		}
		result = ExampleApp().run();
	}
	catch (std::exception & error) {
		OutputDebugStringA(error.what());
		std::cerr << error.what() << std::endl;
	}
	ovr_Shutdown();
	return result;
}
