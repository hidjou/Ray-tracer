#include <SDL.h>
#include <stdio.h>
#include <iostream>
#include <chrono>
#include <fstream>
#include <string.h>
#include <glm/glm.hpp>
#include <algorithm>
#include <vector>
#include "ppl.h"
#include "Shape.h"
#include "Sphere.h"
#include "Ray.h"
#include "Light.h"
#include "Plane.h"
#include "Triangle.h"
#include "objloader.h"
#include "Box.h"
#undef main

// Using glm for vector and maths operators
using namespace std;
using namespace glm;

// Timer declaration
chrono::time_point<chrono::system_clock> start, ending;

// Functions' signature
void userInteractions();
void createObjects();
vec3 traceRay(Ray ray, int traceDepth);
void checkPixel(SDL_Surface* surface);
bool verLine(SDL_Surface* surface, int x, int y, Uint8 r, Uint8 g, Uint8 b);

//---------------------------- SDL vars ---------------------------------

typedef std::chrono::high_resolution_clock Clock;

Uint8* inkeys;
SDL_Event event = { 0 };
Clock::time_point timeNow; //time of current frame
Clock::time_point oldTime; //time of previous frame

//---------------------------- SDL vars ---------------------------------

// Screen resolution
const int width = 640;
const int height = 480;

// Recursive reflection max trace depth
const int maxTraceDepth = 5;

// Feature toggles
bool usingHardShadows = true;
bool usingReflections = true;
bool usingTeapot = true;
bool usingPlane = false;
float softShadowDetailLevel = 1.0f;

// Colours
vec3 black(0, 0, 0);
vec3 white(1, 1, 1);
vec3 red(1, 0, 0);
vec3 green(0, 1, 0);
vec3 blue(0, 0, 1);
vec3 yellow(1, 1, 0);
vec3 beige(0.9, 0.76, 0.46);
vec3 skyBlue(0.050, 0.827, 0.792);

Ray shadowRay;

// Spotlight
Light light(vec3(1, 3, 1), white, 0.9);

// Area light box
Box areaLight(vec3(-4.5, 20, -4.5), vec3(9, 0.1, 9));

// Initializing Object arrays and vectors
vector< vec3 > vertices;
vector< vec3 > normals;
bool res = loadOBJ("teapot_simple_smooth.obj", vertices, normals);
vector<Shape*> shapes;


// Initializing image array
vec3 **image = new vec3*[width];

// Field of view angle
float fieldOfViewAngle = 90;
float fieldOfViewAngleRadians = radians(fieldOfViewAngle);

void userInteractions()
{
	// Handling user interactions
	char choice;
	cout << "" << endl <<"Hello and welcome ! " << endl;
	cout << "" << endl << "to use soft shadows type 'a', to use hard shadows type 'b' !" << endl;
	cin >> choice;

	if (choice == 'a')
	{
		usingHardShadows = false;
		cout << "" << endl << "for medium soft shadow quality type 'a', for high soft shadow quality type 'b' !" << endl;
		cin >> choice;
		if (choice == 'a') softShadowDetailLevel = 1.0f;
		else if (choice == 'b') softShadowDetailLevel = 0.5f;
		else cout << "I did not get that, try again" << endl, userInteractions();
	}
	else if (choice == 'b') usingHardShadows = true;
	else
	{
		cout << "I did not get that, try again" << endl;
		userInteractions();
	}

	cout << "" << endl << "to use reflections type 'a', to not use reflections type 'b' !" << endl;
	cin >> choice;

	if (choice == 'a') usingReflections = true;
	else if (choice == 'b') usingReflections = false;
	else
	{
		cout << "I did not get that, try again" << endl;
		userInteractions();
	}

	cout << "" << endl << "to render Teapot type 'a', to not render Teapot type 'b' !" << endl;
	cin >> choice;

	if (choice == 'a') usingTeapot = true;
	else if (choice == 'b') usingTeapot = false;
	else
	{
		cout << "I did not get that, try again" << endl;
		userInteractions();
	}

	cout << "Press 'r' to turn ON/OF reflections" << endl << endl << "Press 's' to switch between soft shadows and hard shadows" << endl << endl << "Press 't' to render the Teapot" << endl;
	cout << "Press 'p' to render the Plane" << endl;
}

void createObjects()
{
	// Initialize the timer
	start = chrono::system_clock::now();

	//--------------------------------------------- Pushing Spheres, Trianges and Planes --------------------------------------

	shapes.push_back(new Sphere(vec3(0, 0, -20), 4, red, 90));
	shapes.push_back(new Sphere(vec3(5, -1, -15), 2, beige, 150));
	shapes.push_back(new Sphere(vec3(20, 2, -25), 3, blue, 50));
	shapes.push_back(new Sphere(vec3(-5.5, 0, -15), 3, green, 200));
	shapes.push_back(new Sphere(vec3(0, -15, -35), 10, skyBlue, 200));

	shapes.push_back(new Sphere(vec3(7, -8, -20), 3, blue, 50));

	if(usingPlane) shapes.push_back(new Plane(vec3(-10, -10, -25), vec3(1, 1, 0), skyBlue, 50));

	// Pushing Teapot's triangles
	if (usingTeapot)
	{
		for (int i = 0; i < vertices.size(); i += 3)
		{
			shapes.push_back(new Triangle(red, vec3(vertices[i].x, vertices[i].y - 5, vertices[i].z - 10),
				vec3(vertices[i + 1].x, vertices[i + 1].y - 5, vertices[i + 1].z - 10),
				vec3(vertices[i + 2].x, vertices[i + 2].y - 5, vertices[i + 2].z - 10),
				vec3(normals[i]), vec3(normals[i + 1]), vec3(normals[i + 2]), 20));
		}
	}

	//--------------------------------------------- Pushing Spheres, Trianges and Planes --------------------------------------
}

void checkPixel(SDL_Surface* surface)
{
	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
		{
			// Normalized x and y to [0:1]
			float pixelNormalizedx = (i + 0.5) / width;
			float pixelNormalizedy = (j + 0.5) / height;

			// Changing range from [0:1] to [-1:1]
			float pixelRemappedx = 2 * pixelNormalizedx - 1;
			float pixelRemappedy = 1 - 2 * pixelNormalizedy;

			// Changing image aspect ratio
			float imageAspectRatio = width / height;

			pixelRemappedx = pixelRemappedx * imageAspectRatio;
	
			float pixelCamerax = pixelRemappedx * tan(fieldOfViewAngleRadians / 2);
			float pixelCameray = pixelRemappedy * tan(fieldOfViewAngleRadians / 2);

			// Setting camera location
			vec3 pCameraSpace(pixelCamerax, pixelCameray, -1);
			vec3 rayOrigin(0, 0, 0);

			vec3 rayDirection = pCameraSpace - rayOrigin;
			vec3 normalizedRayDirection = normalize(rayDirection);

			Ray ray = Ray(rayOrigin, normalizedRayDirection);

			// Celling the traceRay() function to produce the pixel colour
			vec3 pixelColour = traceRay(ray, 0);
			image[i][j] = pixelColour;

			// Passing the colour values to be mapped of the SDL_surface (screenSurface)
			verLine(surface, i, j, std::min((float)1, pixelColour.x) * 255, std::min((float)1, pixelColour.y) * 255, std::min((float)1, pixelColour.z) * 255);
		}
	}
}

void renderImage()
{
	// Rendering the image to a .ppm file
	std::ofstream ofs("./image.ppm", std::ios::out | std::ios::binary);
	ofs << "P6\n" << width << " " << height << "\n255\n";
	for (unsigned y = 0; y < height; ++y)
	{
		for (unsigned x = 0; x < width; ++x)
		{
			ofs << (unsigned char)(std::min((float)1, (float)image[x][y].x) * 255) <<
				(unsigned char)(std::min((float)1, (float)image[x][y].y) * 255) <<
				(unsigned char)(std::min((float)1, (float)image[x][y].z) * 255);
		}
	}
	ofs.close();
	cout << "Image rendered" << endl;;
}

vec3 traceRay(Ray ray, int traceDepth)
{
	// Initializing and declaring vars to be used
	vec3 reflectedColour, pointColour;
	bool intersection = false;
	int nearestObjectIndex = 0;
	float min_t = INT32_MAX;

	// Background colour to be return in case of no intersection
	vec3 pixelColour = black;

	// Checking intersection
	for (int i = 0; i < shapes.size(); i++)
	{
		if (shapes[i]->intersect(ray) && shapes[i]->t < min_t)
		{
			intersection = true;
			nearestObjectIndex = i;
			min_t = shapes[i]->t;
		}
	}

	if (!intersection)
	{
		return pixelColour;	// If there is no intersection the pixel should be black
	}
	else
	{
		Shape* hitObject = shapes[nearestObjectIndex];

		vec3 lightRay = light.centre - hitObject->pointOfIntersection;
		vec3 reflection = 2.0f * (dot(lightRay, hitObject->getNormal()))*hitObject->getNormal() - lightRay;
		vec3 specularConstant(0.7, 0.7, 0.7);

		float diffuseCoefficient = glm::max(0.0f, dot(normalize(lightRay), normalize(hitObject->getNormal())));
		float specularCoefficient = pow(glm::max(0.0f, dot(normalize(reflection), normalize(lightRay))), hitObject->shininess);

		vec3 ambientComponent = light.intensity * vec3(1);
		vec3 diffuseComponent = hitObject->colour * light.colour * diffuseCoefficient;
		vec3 specularComponent = specularCoefficient * specularConstant;

		// Combining components to obtain the phong shaded pixel colour
		pointColour = (diffuseComponent + specularComponent) * ambientComponent;

		//----------------------------------------- Reflections ------------------------------------------------

		// Checking object's reflectiveness and traceDepth's size
		if ((traceDepth < maxTraceDepth) && (hitObject->shininess > 0) && usingReflections)
		{
			vec3 reflectedRayOrigin = hitObject->pointOfIntersection + hitObject->getNormal() * 0.01f;
			vec3 reflectedRayDirection = ray.direction - 2.0f * dot(ray.direction, normalize(hitObject->getNormal()))*normalize(hitObject->getNormal());
			Ray reflectionRay = Ray(reflectedRayOrigin, reflectedRayDirection);

			// Calling the function to check for nearby objects
			reflectedColour = ((float)hitObject->shininess / 150) * traceRay(reflectionRay, traceDepth + 1);
		}

		// Combining reflection colour with phong colour
		pixelColour = pointColour + reflectedColour;

		//----------------------------------------- Reflections ------------------------------------------------

		//----------------------------------------- Shadows ----------------------------------------------------

		//sphere shadow
		shadowRay.direction = normalize(light.centre - hitObject->pointOfIntersection) + hitObject->getNormal() * 0.01f;
		shadowRay.origin = (hitObject->pointOfIntersection + hitObject->getNormal() * 0.01f);

		////teapot shadow
		//shadowRay.direction = light.centre - (hitObject->pointOfIntersection + hitObject->getNormal() * (float)0.01);
		//shadowRay.origin = normalize(hitObject->pointOfIntersection + hitObject->getNormal() * (float)0.01);

		// Soft shadow variables
		float shadeIncrement = softShadowDetailLevel;
		float shadowRayHits = 0.0f;
		float totalShadowRays = (areaLight.size.x / shadeIncrement) * (areaLight.size.z / shadeIncrement);
		float softShadowCoefficient = 1.0f;

		// Checking hard shadows toggle
		if (usingHardShadows)
		{
		//----------------------------------------- Hard shadows -----------------------------------------------

			for (int i = 0; i < shapes.size(); i++)
			{
				// Checking the shadow ray's intersection with any object that might block light
				if (shapes[i]->intersect(shadowRay))
				{
					// If in shadow dim down colour
					return pixelColour * 0.2f;
				}
			}
			// If not in shadow return colour
			return pixelColour;

		//----------------------------------------- Hard shadows -----------------------------------------------
		}
		else
		{
		//----------------------------------------- Soft shadows -----------------------------------------------

			

			// Iterating through the X and Z axis of the area light box
			for (float i = 0; i < areaLight.size.x; i += shadeIncrement)
			{
				for (float j = 0; j < areaLight.size.z; j += shadeIncrement)
				{
					// Reseting the shadow ray origin and direction so that if moves through the area light box
					shadowRay.origin = hitObject->pointOfIntersection + hitObject->getNormal()*0.01f;
					shadowRay.direction = normalize(light.centre - hitObject->pointOfIntersection + hitObject->getNormal() * 0.01f);

					// normalize(light.centre - hitObject->pointOfIntersection + hitObject->getNormal() * 0.01f) or normalize(light.centre - hitObject->pointOfIntersection) + hitObject->getNormal() * 0.01f

					float min_d = INT32_MAX;
					bool shapeHit = false;

					// Moving around the area light's box
					light.centre.x = i;
					light.centre.z = j;

					for (int k = 0; k < shapes.size(); k++)
					{
						// Checking shadow ray intersection with shapes
						if (shapes[k]->intersect(shadowRay) && hitObject->t < min_d)
						{
							min_d = shapes[k]->t;
							shapeHit = true;
							shadowRayHits += 1;
						}
					}
					if (shapeHit)
					{
						softShadowCoefficient = 1 - (shadowRayHits / totalShadowRays);
					}
				}
			}
			return pixelColour * softShadowCoefficient;

		//----------------------------------------- Soft shadows -----------------------------------------------
		}
		//----------------------------------------- Shadows ----------------------------------------------------
	}
	return pixelColour;
}

bool verLine(SDL_Surface* surface, int x, int y, Uint8 r, Uint8 g, Uint8 b)
{
	// No single point of the line is on screen
	if (y < 0 || y >= height || x < 0 || x >= width) return 0;

	Uint32 colorSDL = SDL_MapRGB(surface->format, r, g, b);
	Uint32* bufp;

	bufp = (Uint32*)surface->pixels + y * surface->pitch / 4 + x;
	*bufp = colorSDL;

	return 1;
}

// Delay makes CPU have some free time, use once per frame to avoid 100% usage of a CPU core
bool done(bool quit_if_esc, bool delay)
{
	// So it consumes less processing power
	if (delay) SDL_Delay(5);
	int done = 0;
	if (!SDL_PollEvent(&event)) return 0;

	if (event.type == SDL_QUIT)
	{
		done = 1;
	}
	// User presses a key
	else if (event.type == SDL_KEYDOWN)
	{
		// Select surfaces based on key press
		cout << event.key.keysym.sym << endl;
		switch (event.key.keysym.sym)
		{
		case SDLK_ESCAPE:
			done = 1;
			break;

		default:
			break;
		}
	}

	return done;
}

int main()
{
	// Filling the image 2D array
	for (int i = 0; i < width; i++)
	{
		image[i] = new vec3[height];
	}

	// Take user input to adjust the scene
	userInteractions();

	// The window we'll be rendering to
	SDL_Window* window = NULL;

	// The surface contained by the window
	SDL_Surface* screenSurface = NULL;

	// Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
	}
	else
	{
		// Create window
		window = SDL_CreateWindow("Scene", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN);
		if (window == NULL)
		{
			printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
		}
		else
		{
			// Get window surface
			screenSurface = SDL_GetWindowSurface(window);

			while (!done(true, false))
			{
				// Clear the surface
				SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0x00, 0x00, 0x00));

				// Create scene objects
				createObjects();
				
				// Iterate through screen and shade it
				checkPixel(screenSurface);

				// Render image file
				//renderImage();

				// Update the surface
				SDL_UpdateWindowSurface(window);

				// Timer ending
				ending = chrono::system_clock::now();
				chrono::duration<double> elapsed_seconds = ending - start;
				time_t end_time = chrono::system_clock::to_time_t(ending);
				cout << "Elapsed time = " << elapsed_seconds.count() << "   Updating surface !!" << endl;


				if (event.type == SDL_KEYDOWN)
				{
					// Event listening
					switch (event.key.keysym.sym)
					{
					case SDLK_s:
					{
						cout << "Switching shadow mode, Please Wait ! " << endl;
						usingHardShadows = !usingHardShadows;
						break;
					}
					case SDLK_t:
					{
						cout << "Rendering Teapot, Please Wait ! " << endl;
						usingTeapot = !usingTeapot;
						break;
					}
					case SDLK_r:
					{
						cout << "Toggling reflections, Please Wait ! " << endl;
						usingReflections = !usingReflections;
						break;
					}
					case SDLK_p:
					{
						cout << "Rendering Plane, Please Wait ! " << endl;
						usingPlane = !usingPlane;
						break;
					}
					default:
						break;
					}
				}
			}
		}
	}

	// Destroy window
	SDL_DestroyWindow(window);

	// Quit SDL subsystems
	SDL_Quit();

	// Keeping console window open
	system("PAUSE");
	return 0;
}