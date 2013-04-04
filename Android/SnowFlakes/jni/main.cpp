#include <android_native_app_glue.h>
#include <android/sensor.h>
#include <android/log.h>

#include <errno.h>
#include <math.h>
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#include "matrix4x4f.h"
#include "vector3f.h"
#include "texture.h"

using namespace guildhall;

#define LOG_TAG "PNGLoader"
#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__))

// Our saved state data.
struct savedState
{
	float angle;
	int32_t x;
	int32_t y;
};

// Our app's user data.
struct engine
{
	struct android_app* app;

	ASensorManager* sensorManager;
	const ASensor* accelerometerSensor;
	ASensorEventQueue* sensorEventQueue;

	int animating;
	EGLDisplay display;
	EGLSurface surface;
	EGLContext context;
	int32_t width;
	int32_t height;
	struct savedState state;
};

double g_nowTime, g_prevTime;

//
// Vertex data in separate arrays.
//

const GLfloat g_vertices[] =
{
	 1.0f,  1.0f, 0.0f,
	-1.0f,  1.0f, 0.0f,
	-1.0f, -1.0f, 0.0f,

	 1.0f,  1.0f, 0.0f,
	-1.0f, -1.0f, 0.0f,
	 1.0f, -1.0f, 0.0f
};
/*
const GLfloat g_colors[] =
{
	1.0f, 0.0f, 0.0f, 1.0f, // vertex 0 is red
	0.0f, 1.0f, 0.0f, 1.0f, // vertex 1 is green
	0.0f, 0.0f, 1.0f, 1.0f, // vertex 2 is blue

	1.0f, 0.0f, 0.0f, 1.0f, // vertex 3 is red
	0.0f, 0.0f, 1.0f, 1.0f, // vertex 4 is blue
	1.0f, 1.0f, 1.0f, 1.0f  // vertex 5 is white
};
//*/
//*
const GLfloat g_colors[] =
{
	1.0f, 1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f, 1.0f,

	1.0f, 1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f, 1.0f
};
//*/
const GLfloat g_texCoords[] =
{
	1.0, 1.0,
	0.0, 1.0,
	0.0, 0.0,

	1.0, 1.0,
	0.0, 0.0,
	1.0, 0.0
};

//
// Shader related variables...
//

Texture* g_texture = NULL;
Matrix4x4f g_perspectiveMatrix;

GLuint g_program;
GLuint g_a_positionHandle;
GLuint g_a_colorHandle;
GLuint g_a_texCoordHandle;
GLuint g_u_mvpMatrixHandle;
GLuint g_u_texture0Handle;

static const char g_vertexShader[] =
	"attribute vec4 a_position;\n"
	"attribute vec4 a_color;\n"
	"attribute vec2 a_texCoord;\n"
	"uniform mat4 u_mvpMatrix;\n"
	"varying vec4 v_color;\n"
	"varying vec2 v_texCoord;\n"

	"void main()\n"
    "{\n"
	"    gl_Position = u_mvpMatrix * a_position;"
	"    v_color = a_color;"
	"    v_texCoord = a_texCoord;"
	"}\n";

static const char g_fragmentShader[] =
	"precision mediump float;\n"
	"varying vec4 v_color;"
	"varying lowp vec2 v_texCoord;\n"
	"uniform sampler2D u_texture0;\n"

	"void main()\n"
	"{\n"
	"    gl_FragColor = v_color * texture2D( u_texture0, v_texCoord );\n"
	"}\n";

static void printGLString( const char *name, GLenum s )
{
	const char *v = (const char *) glGetString( s );
	LOGI( "GL %s = %s\n", name, v );
}

static double getCurrentTimeInSeconds()
{
   timespec lTimeVal;
   clock_gettime( CLOCK_MONOTONIC, &lTimeVal );
   return lTimeVal.tv_sec + (lTimeVal.tv_nsec * 1.0e-9);
}

GLuint loadShader( GLenum shaderType, const char* pSource )
{
	GLuint shader = glCreateShader( shaderType );

	if( shader )
	{
		glShaderSource( shader, 1, &pSource, NULL );
		glCompileShader( shader );

		GLint compiled = 0;
		glGetShaderiv( shader, GL_COMPILE_STATUS, &compiled );

		if( !compiled )
		{
			GLint infoLen = 0;
			glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &infoLen );

			if( infoLen )
			{
				char* buf = (char*) malloc( infoLen );

				if( buf )
				{
					glGetShaderInfoLog( shader, infoLen, NULL, buf );
					LOGE( "Could not compile shader %d:\n%s\n", shaderType, buf );
					free( buf );
				}

				glDeleteShader( shader );
				shader = 0;
			}
		}
	}

	return shader;
}

GLuint createProgram( const char* pVertexSource, const char* pFragmentSource )
{
	GLuint vertexShader = loadShader( GL_VERTEX_SHADER, pVertexSource );

	if( !vertexShader )
		return 0;

	GLuint pixelShader = loadShader( GL_FRAGMENT_SHADER, pFragmentSource );

	if( !pixelShader )
		return 0;

	GLuint program = glCreateProgram();

	if( program )
	{
		glAttachShader( program, vertexShader );
		glAttachShader( program, pixelShader );

		glLinkProgram( program );
		GLint linkStatus = GL_FALSE;
		glGetProgramiv( program, GL_LINK_STATUS, &linkStatus );

		if( linkStatus != GL_TRUE )
		{
			GLint bufLength = 0;
			glGetProgramiv( program, GL_INFO_LOG_LENGTH, &bufLength );

			if( bufLength )
			{
				char* buf = (char*) malloc( bufLength );

				if( buf )
				{
					glGetProgramInfoLog( program, bufLength, NULL, buf );
					LOGE( "Could not link program:\n%s\n", buf );
					free( buf );
				}
			}

			glDeleteProgram( program );
			program = 0;
		}
	}

	return program;
}

static int initGL( struct engine* engine )
{
	// initialize OpenGL ES and EGL

	//
	// Here specify the attributes of the desired configuration.
	// Below, we select an EGLConfig with at least 8 bits per color
	// that supports OpenGL ES 2.0.
	//

	const EGLint attribs[] =
	{
		EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
		EGL_BLUE_SIZE, 8,
		EGL_GREEN_SIZE, 8,
		EGL_RED_SIZE, 8,
		EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
		EGL_NONE
	};

	EGLint w, h, dummy, format;
	EGLint numConfigs;
	EGLConfig config;
	EGLSurface surface;
	EGLContext context;

	EGLDisplay display = eglGetDisplay( EGL_DEFAULT_DISPLAY );

	eglInitialize( display, 0, 0 );

	//
	// Here, the application chooses the configuration it desires. In this
	// sample, we have a very simplified selection process, where we pick
	// the first EGLConfig that matches our criteria.
	//

	eglChooseConfig( display, attribs, &config, 1, &numConfigs );

	//
	// EGL_NATIVE_VISUAL_ID is an attribute of the EGLConfig that is
	// guaranteed to be accepted by ANativeWindow_setBuffersGeometry().
	// As soon as we picked a EGLConfig, we can safely reconfigure the
	// ANativeWindow buffers to match, using EGL_NATIVE_VISUAL_ID.
	//

	eglGetConfigAttrib( display, config, EGL_NATIVE_VISUAL_ID, &format );

	ANativeWindow_setBuffersGeometry( engine->app->window, 0, 0, format );

	surface = eglCreateWindowSurface( display, config, engine->app->window, NULL );

	const EGLint attrib_list [] =
	{
		EGL_CONTEXT_CLIENT_VERSION, 2,
		EGL_NONE
	};

	context = eglCreateContext( display, config, NULL, attrib_list );

	if( eglMakeCurrent( display, surface, surface, context ) == EGL_FALSE )
	{
		LOGW( "Unable to eglMakeCurrent" );
		return -1;
	}

	eglQuerySurface( display, surface, EGL_WIDTH, &w );
	eglQuerySurface( display, surface, EGL_HEIGHT, &h );

	g_texture = new Texture( engine->app, "android.png" );
	g_texture->load();

	engine->display = display;
	engine->context = context;
	engine->surface = surface;
	engine->width = w;
	engine->height = h;
	engine->state.angle = 0;

	// Initialize GL state.
	glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST );
	glDisable( GL_DEPTH_TEST );

	printGLString( "Version", GL_VERSION );
	printGLString( "Vendor", GL_VENDOR );
	printGLString( "Renderer", GL_RENDERER );
	printGLString( "Extensions", GL_EXTENSIONS );

	g_program = createProgram( g_vertexShader, g_fragmentShader );

	if( !g_program )
	{
		LOGE( "Could not create program." );
		return false;
	}

	// Vertex shader variables
	g_a_positionHandle = glGetAttribLocation( g_program, "a_position" );
	g_a_colorHandle = glGetAttribLocation( g_program, "a_color" );
	g_a_texCoordHandle = glGetAttribLocation( g_program, "a_texCoord" );
	g_u_mvpMatrixHandle = glGetUniformLocation ( g_program, "u_mvpMatrix" );

	// Fragment shader variables
	g_u_texture0Handle = glGetUniformLocation( g_program, "u_texture0" );

	glViewport( 0, 0, w, h );

	//gluPerspective( 45.0f, (float)w / (float)h, 0.1f, 100.0f );
	g_perspectiveMatrix = Matrix4x4f::createPerspectiveProjection( 45.0f, (float)w / (float)h, 0.1f, 100.0f );

	g_nowTime = getCurrentTimeInSeconds();
	g_prevTime = g_nowTime;

	return 0;
}

static void draw( struct engine* engine )
{
	if( engine->display == NULL )
	{
		// No display.
		return;
	}

	g_nowTime = getCurrentTimeInSeconds();
	double elapsed = g_nowTime - g_prevTime;

	glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
	glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT );

	glUseProgram( g_program );

	static float rotationY = 0.0f;
	rotationY += (elapsed * 50.0f);

	if( rotationY > 360.0f )
		rotationY = 0.0f;

	Matrix4x4f translationMatrixZ;
	translationMatrixZ.translate_z( -6.0 );

	Matrix4x4f rotationMatrixY;
	rotationMatrixY.rotate_y( rotationY );

	Matrix4x4f mvp = g_perspectiveMatrix * translationMatrixZ * rotationMatrixY;

	glUniformMatrix4fv( g_u_mvpMatrixHandle, 1, GL_FALSE, mvp.m );

	glVertexAttribPointer( g_a_positionHandle, 3, GL_FLOAT, GL_FALSE, 0, g_vertices );
	glEnableVertexAttribArray( g_a_positionHandle );

	glVertexAttribPointer( g_a_colorHandle, 4, GL_FLOAT, GL_FALSE, 0, g_colors );
	glEnableVertexAttribArray( g_a_colorHandle );

	glVertexAttribPointer( g_a_texCoordHandle, 2, GL_FLOAT, GL_FALSE, 0, g_texCoords );
	glEnableVertexAttribArray( g_a_texCoordHandle );

	glUniform1i( g_u_texture0Handle, 0 );
	g_texture->apply();

	glDrawArrays( GL_TRIANGLES, 0, 6 );

	eglSwapBuffers( engine->display, engine->surface );

	g_prevTime = g_nowTime;
}

static void shutdownGL( struct engine* engine )
{
	if( engine->display != EGL_NO_DISPLAY )
	{
		eglMakeCurrent( engine->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT );

		if( engine->context != EGL_NO_CONTEXT )
			eglDestroyContext( engine->display, engine->context );

		if( engine->surface != EGL_NO_SURFACE )
			eglDestroySurface( engine->display, engine->surface );

		eglTerminate( engine->display );
	}

	engine->animating = 0;
	engine->display = EGL_NO_DISPLAY;
	engine->context = EGL_NO_CONTEXT;
	engine->surface = EGL_NO_SURFACE;
}

// Process the next input event.
static int32_t handleInputEvent( struct android_app* app, AInputEvent* event )
{
	struct engine* engine = (struct engine*) app->userData;

	if( AInputEvent_getType( event ) == AINPUT_EVENT_TYPE_MOTION )
	{
		engine->animating = 1;
		engine->state.x = AMotionEvent_getX( event, 0 );
		engine->state.y = AMotionEvent_getY( event, 0 );

		return 1;
	}
	return 0;
}

// Process the next application command.
static void handleAppCmd( struct android_app* app, int32_t cmd )
{
	struct engine* engine = (struct engine*) app->userData;

	switch( cmd )
	{
		case APP_CMD_SAVE_STATE:
		{
			// The system has asked us to save our current state.  Do so.
			engine->app->savedState = malloc( sizeof(struct savedState) );
			*((struct savedState*) engine->app->savedState) = engine->state;
			engine->app->savedStateSize = sizeof(struct savedState);
		}
		break;

		case APP_CMD_INIT_WINDOW:
		{
			// The window is being shown, get it ready.
			if( engine->app->window != NULL )
			{
				initGL( engine );
				draw( engine );
			}
		}
		break;

		case APP_CMD_TERM_WINDOW:
		{
			// The window is being hidden or closed, clean it up.
			shutdownGL( engine );
		}
		break;

		case APP_CMD_GAINED_FOCUS:
		{
			// When our app gains focus, we start monitoring the accelerometer.
			if( engine->accelerometerSensor != NULL )
			{
				ASensorEventQueue_enableSensor( engine->sensorEventQueue, engine->accelerometerSensor );

				// We'd like to get 60 events per second (in us).
				ASensorEventQueue_setEventRate( engine->sensorEventQueue, engine->accelerometerSensor, (1000L / 60) * 1000 );
			}
		}
		break;

		case APP_CMD_LOST_FOCUS:
		{
			// When our app loses focus, we stop monitoring the accelerometer.
			// This is to avoid consuming battery while not being used.
			if( engine->accelerometerSensor != NULL )
				ASensorEventQueue_disableSensor( engine->sensorEventQueue, engine->accelerometerSensor );

			// Also stop animating.
			engine->animating = 0;
			draw( engine );
		}
		break;
	}
}

// This is the main entry point of a native application that is using
// android_native_app_glue.  It runs in its own thread, with its own
// event loop for receiving input events and doing other things.
void android_main( struct android_app* app )
{
	struct engine engine;

	// Make sure glue isn't stripped.
	app_dummy();

	memset( &engine, 0, sizeof(engine) );
	engine.app = app;
	engine.animating = 1;

	app->userData = &engine;
	app->onAppCmd = handleAppCmd;
	app->onInputEvent = handleInputEvent;

	// Prepare to monitor accelerometer
	engine.sensorManager = ASensorManager_getInstance();
	engine.accelerometerSensor = ASensorManager_getDefaultSensor( engine.sensorManager, ASENSOR_TYPE_ACCELEROMETER );
	engine.sensorEventQueue = ASensorManager_createEventQueue( engine.sensorManager, app->looper, LOOPER_ID_USER, NULL, NULL );

	if( app->savedState != NULL )
	{
		// We are starting with a previous saved state; restore from it.
		engine.state = *(struct savedState*) app->savedState;
	}

	// Loop forever and wait for stuff to do, until we get a destroy request.

	while( 1 )
	{
		// Read all pending events.
		int ident;
		int events;
		struct android_poll_source* source;

		// If not animating, we will block forever waiting for events.
		// If animating, we loop until all events are read, then continue
		// to draw the next frame of animation.
		while( (ident = ALooper_pollAll( engine.animating ? 0 : -1, NULL, &events, (void**)&source ) ) >= 0 )
		{
			// Process this event.
			if( source != NULL )
				source->process( app, source );

			// If a sensor has data, process it now.
			if( ident == LOOPER_ID_USER )
			{
				if( engine.accelerometerSensor != NULL )
				{
					ASensorEvent event;

					while( ASensorEventQueue_getEvents( engine.sensorEventQueue, &event, 1 ) > 0 )
					{
						//LOGI( "accelerometer: x=%f y=%f z=%f", event.acceleration.x, event.acceleration.y, event.acceleration.z );
					}
				}
			}

			// Check if we are exiting.
			if( app->destroyRequested != 0 )
			{
				shutdownGL( &engine );
				return;
			}
		}

		if( engine.animating )
		{
			// Done with events - draw next animation frame.
			engine.state.angle += 0.01f;

			if( engine.state.angle > 1 )
				engine.state.angle = 0;

			// Drawing is throttled to the screen update rate, so there
			// is no need to do timing here.
			draw( &engine );
		}
	}
}

