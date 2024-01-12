#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <iostream>
#include <string>
#include <iostream>
#include <fstream>
#include "Utils.h"
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <ft2build.h>
#include <map>
#include FT_FREETYPE_H  

using namespace std;

//Correct window size
//Create a menu
//history list
//looks at collision history and figures out text to display
//text input?
//Have to go in a certain order?
//Remember details about each character
//Randomly going to switch cubes 
//Story based
//Can't switch until you complete a certain task
//If you get to a certain line of dialogue you can switch cubes

#define numVAOs 1
#define numVBO 3

GLuint renderingProgram, renderingProgram2;

int flags;

struct Character {
	unsigned int textureID;  // ID handle of the glyph texture
	glm::ivec2   Size;       // Size of glyph
	glm::ivec2   Bearing;    // Offset from baseline to left/top of glyph
	unsigned int Advance;    // Offset to advance to next glyph
};

std::map<char, Character> Characters;

float cameraX, cameraY, cameraZ;
glm::vec4 cameraVX, cameraVY, cameraVZ;

float pLocX[6] = { 0.0f, 10.0f, 5.0f, -3.0f, 0.0f, -10.0f };
float pLocY[6] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
float pLocZ[6] = { 0.0f, -5.0f, -8.0f, -10.0f, -14.0f, -3.0f };

//float historyList[6][100];

bool startedGame;
bool firstWhite;
bool firstTime;
bool afterR1;

int active;

int counter;
int dialogue;

//Is character moving?
bool movingLeft, movingRight, movingForward, movingBack;
int stateA, stateS, stateD, stateW, stateE, stateSpace, stateY, stateN;

//Is E being pressed?
bool isPressedE, isPressedSpace, isPressedY, isPressedN;

bool draw1, draw2, draw1a, draw2a, draw1b, draw2b, draw1c, draw2c, draw1d, draw2d, draw1e, draw2e, draw1f, draw2f, draw1g, draw2g, draw1h, draw2h;
bool draw1i, draw2i, draw1j, draw2j, draw1k, draw2k, draw1l, draw2l, draw1m, draw2m, draw1n, draw2n, draw1o, draw2o, draw1p, draw2p, draw1q, draw2q;
int decisions[25];

bool beenCube[6] = { true, false, false, false, false, false };

bool finishedRound;

bool noSwitch;

bool titleScreen;

GLuint vao[numVAOs];
GLuint vbo[numVBO];

GLuint mvLoc, projLoc, colorLoc, textLoc, textColorLoc, projectionLoc;
int width, height;
float aspect;
glm::mat4 pMat, vMat, mMat, mvMat;

double firstPressTime;
double firstPressTime2;
double secondPressTime, timeCheck1, timeCheck2;

void APIENTRY glDebugOutput(GLenum source,
	GLenum type,
	unsigned int id,
	GLenum severity,
	GLsizei length,
	const char* message,
	const void* userParam)
{
	// ignore non-significant error/warning codes
	if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

	std::cout << "---------------" << std::endl;
	std::cout << "Debug message (" << id << "): " << message << std::endl;

	switch (source)
	{
	case GL_DEBUG_SOURCE_API:             std::cout << "Source: API"; break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   std::cout << "Source: Window System"; break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cout << "Source: Shader Compiler"; break;
	case GL_DEBUG_SOURCE_THIRD_PARTY:     std::cout << "Source: Third Party"; break;
	case GL_DEBUG_SOURCE_APPLICATION:     std::cout << "Source: Application"; break;
	case GL_DEBUG_SOURCE_OTHER:           std::cout << "Source: Other"; break;
	} std::cout << std::endl;

	switch (type)
	{
	case GL_DEBUG_TYPE_ERROR:               std::cout << "Type: Error"; break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cout << "Type: Deprecated Behaviour"; break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cout << "Type: Undefined Behaviour"; break;
	case GL_DEBUG_TYPE_PORTABILITY:         std::cout << "Type: Portability"; break;
	case GL_DEBUG_TYPE_PERFORMANCE:         std::cout << "Type: Performance"; break;
	case GL_DEBUG_TYPE_MARKER:              std::cout << "Type: Marker"; break;
	case GL_DEBUG_TYPE_PUSH_GROUP:          std::cout << "Type: Push Group"; break;
	case GL_DEBUG_TYPE_POP_GROUP:           std::cout << "Type: Pop Group"; break;
	case GL_DEBUG_TYPE_OTHER:               std::cout << "Type: Other"; break;
	} std::cout << std::endl;

	switch (severity)
	{
	case GL_DEBUG_SEVERITY_HIGH:         std::cout << "Severity: high"; break;
	case GL_DEBUG_SEVERITY_MEDIUM:       std::cout << "Severity: medium"; break;
	case GL_DEBUG_SEVERITY_LOW:          std::cout << "Severity: low"; break;
	case GL_DEBUG_SEVERITY_NOTIFICATION: std::cout << "Severity: notification"; break;
	} std::cout << std::endl;
	std::cout << std::endl;
}

void setupVertices(void) {
	float vertexPositions[108] = {
	 -1.0f, 1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f, -1.0f,
	 1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f,
	 1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
	 -1.0f, -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
	 -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, 1.0f,
	 -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f,
	 -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,
	 -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f,
	 1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f, -1.0f
	};

	float groundPositions[18] = {
		-100.0f, -2.0f, -100.0f, 100.0f, -2.0f, -100.0f, -100.0f, -2.0f, 100.0f,
		-100.0f, -2.0f, 100.0f, 100.0f, -2.0f, -100.0f, 100.0f, -2.0f, 100.0f
	};

	glGenVertexArrays(1, vao);
	glBindVertexArray(vao[0]);
	glGenBuffers(numVBO, vbo);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexPositions), vertexPositions, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(groundPositions), groundPositions, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
}

int initType()
{
	FT_Library ft;
	if (FT_Init_FreeType(&ft))
	{
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
		return -1;
	}

	FT_Face face;
	if (FT_New_Face(ft, "C:/Users/chloe/OneDrive/Documents/CMSC 699/IndepStudyGame/StarterProject/lib/Fonts/arial.ttf", 0, &face))
	{
		std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
		return -1;
	}

	FT_Set_Pixel_Sizes(face, 0, 48);

	if (FT_Load_Char(face, 'X', FT_LOAD_RENDER))
	{
		std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
		return -1;
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction

	for (unsigned char c = 0; c < 128; c++)
	{
		// load character glyph 
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
			continue;
		}
		// generate texture
		unsigned int texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			face->glyph->bitmap.width,
			face->glyph->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			face->glyph->bitmap.buffer
		);
		// set texture options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// now store character for later use
		Character character = {
			texture,
			glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			face->glyph->advance.x
		};
		Characters.insert(std::pair<char, Character>(c, character));
	}
	glBindTexture(GL_TEXTURE_2D, 0);

	FT_Done_Face(face);
	FT_Done_FreeType(ft);
}

void init(GLFWwindow* window) {
	renderingProgram = Utils::createShaderProgram("vertShader.glsl", "fragShader.glsl");
	renderingProgram2 = Utils::createShaderProgram("vertShader_text.glsl", "fragShader_text.glsl");

	glfwGetFramebufferSize(window, &width, &height);
	aspect = (float)width / (float)height;
	pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f);

	cameraX = 0.0f; cameraY = 10.0f; cameraZ = 15.0f;

	float pLocX[6] = { 0.0f, 10.0f, 5.0f, -3.0f, 0.0f, -10.0f };
	float pLocY[6] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
	float pLocZ[6] = { 0.0f, -5.0f, -8.0f, -10.0f, -14.0f, -3.0f };

	bool beenCube[6] = { true, false, false, false, false, false }; 

	active = 0; 

	counter = 0;

	isPressedE = false;
	isPressedSpace = false;
	isPressedY = false;
	isPressedN = false;

	dialogue = -1;

	firstPressTime = 0;
	firstPressTime2 = 0;
	secondPressTime = 0;
	
	movingRight = false; movingLeft = false; 
	movingBack = false; movingForward = false;

	startedGame = false;
	firstWhite = false;
	firstTime = true;
	afterR1 = false;

	titleScreen = true;

	draw1 = false; draw2 = false; draw1a = false; draw2a = false; draw1b = false; draw2b = false;
	draw1c = false; draw2c = false; draw1d = false; draw2d = false; draw1e = false; draw2e = false;
	draw1f = false; draw2f = false; draw1g = false; draw2g = false; draw1h = false; draw2h = false;
	draw1i = false; draw2i = false; draw1j = false; draw2j = false; draw1k = false; draw2k = false;

	finishedRound = false;

	cameraVX = glm::vec4(1.0, 0.0, 0.0, 0.0);
	cameraVY = glm::vec4(0.0, 1.0, 0.0, 0.0);
	cameraVZ = glm::vec4(0.0, 0.0, 1.0, 0.0);

	glm::mat4 rMat2 = glm::rotate(glm::mat4(1.0f), float(-3.14159 / 9), glm::vec3(cameraVX));
	cameraVY = rMat2 * cameraVY;
	cameraVZ = rMat2 * cameraVZ;

	setupVertices();
	initType();
}

bool CheckCollision(float& secondCubeX, float& secondCubeZ) // AABB - AABB collision
{
	// collision x-axis?
	bool collisionX = pLocX[active] + 1.0f >= secondCubeX - 1.0f &&
		secondCubeX + 1.0f >= pLocX[active] - 1.0f;
	// collision z-axis?
	bool collisionZ = pLocZ[active] + 1.0f >= secondCubeZ - 1.0f &&
		secondCubeZ + 1.0f >= pLocZ[active] - 1.0f;
	// collision only if on both axes
	return collisionX && collisionZ;
}

bool closeEnough(const float& a, const float& b) {
	const float eps = 1e-8;
	float abs = a - b;
	if (abs < 0) abs = -abs;
	return abs < eps;
}

void drawText(std::string text, float x, float y, float scale, glm::vec3 color)
{
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glUseProgram(renderingProgram2);
	projectionLoc = glGetUniformLocation(renderingProgram2, "projection");
	//textLoc = glGetUniformLocation(renderingProgram2, "text");
	textColorLoc = glGetUniformLocation(renderingProgram2, "textColor");

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
	glBindVertexArray(vao[0]);

	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glm::mat4 projection = glm::ortho(0.0f, 600.0f, 0.0f, 600.0f);

	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	glUniform3f(textColorLoc, color.x, color.y, color.z);
	//glUniform1i(textLoc, 0);
	glActiveTexture(GL_TEXTURE0);

	// iterate through all characters
	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++)
	{
		Character ch = Characters[*c];

		float xpos = x + ch.Bearing.x * scale;
		float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

		float w = ch.Size.x * scale;
		float h = ch.Size.y * scale;
		// update VBO for each character
		float vertices[6][4] = {
			{ xpos,     ypos + h,   0.0f, 0.0f },
			{ xpos,     ypos,       0.0f, 1.0f },
			{ xpos + w, ypos,       1.0f, 1.0f },

			{ xpos,     ypos + h,   0.0f, 0.0f },
			{ xpos + w, ypos,       1.0f, 1.0f },
			{ xpos + w, ypos + h,   1.0f, 0.0f }
		};
		// render glyph texture over quad
		glBindTexture(GL_TEXTURE_2D, ch.textureID);
		//glUniform1i(textLoc, ch.textureID);
		// update content of VBO memory
		glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);

		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
		
		// render quad
		glDrawArrays(GL_TRIANGLES, 0, 6);
		// now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
	}
	glBindVertexArray(vao[0]);
	glBindTexture(GL_TEXTURE_2D, 0);

	glDisable(GL_BLEND);
	glDisable(GL_CULL_FACE);

	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(glDebugOutput, nullptr);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);

}

void drawCube(const int& cubeNum, double currentTime)
{	
	//glClear(GL_DEPTH_BUFFER_BIT);
	glUseProgram(renderingProgram);

	if (movingLeft == true && cubeNum == active && pLocX[cubeNum] > -10)
	{
		pLocX[cubeNum] -= 0.2;
		dialogue = -1;
	}

	if (movingRight == true && cubeNum == active && pLocX[cubeNum] < 10)
	{
		pLocX[cubeNum] += 0.2;
		dialogue = -1;
	}

	if (movingForward == true && cubeNum == active && pLocZ[cubeNum] > -20)
	{
		pLocZ[cubeNum] -= 0.2;
		dialogue = -1;
	}

	if (movingBack == true && cubeNum == active && pLocZ[cubeNum] < 2 )
	{
		pLocZ[cubeNum] += 0.2;
		dialogue = -1; 
	}

	mMat = glm::translate(glm::mat4(1.0f), glm::vec3(pLocX[cubeNum], pLocY[cubeNum], pLocZ[cubeNum]));
	mvMat = vMat * mMat;

	glEnable(GL_DEPTH_TEST);
	//glDepthFunc(GL_LEQUAL);

	aspect = (float)width / (float)height;
	pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f); // 1.0472 radians = 60 degrees

	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	/*
	if (cubeNum == active)
	{
		glUniform3f(colorLoc, 1.0, 0.0, 0.0); //red	
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
	else*/
	{
		switch (cubeNum)
		{		
			case 0: 
				glUniform3f(colorLoc, 1.0, 0.0, 0.0); //red
				break;
			case 1: 
				glUniform3f(colorLoc, 0.0, 0.5, 0.5); //teal
				break;
			case 2:
				glUniform3f(colorLoc, 0.5, 0.3, 0.6); //purple
				break;
			case 3:
				glUniform3f(colorLoc, 0.6, 0.0, 0.3); //dark pink
				break;
			case 4:
				glUniform3f(colorLoc, 0.0, 0.5, 0.0); //green
				break;
			case 5:
				glUniform3f(colorLoc, 1.0, 0.5, 0.0); //orange
				break;
		}
		//Detect collision
		if (CheckCollision(pLocX[cubeNum], pLocZ[cubeNum]) && cubeNum != active)
		{
			glUniform3f(colorLoc, 1.0, 1.0, 1.0); //white	
			if (firstTime == true)
			{
				firstWhite = true;
			}
			
			if (isPressedE && secondPressTime - firstPressTime >= 0.5 && finishedRound && beenCube[cubeNum] == false)
			{
				firstPressTime = currentTime;
				active = cubeNum;
				beenCube[cubeNum] = true;
				//glUniform3f(colorLoc, 1.0, 0.0, 0.0); //red	
				firstTime = false;
				finishedRound = false;
			}

			if ((isPressedE && secondPressTime - firstPressTime >= 0.5 && finishedRound && beenCube[cubeNum] == true))
			{
				noSwitch = true;
			}

			if (isPressedSpace)// && secondPressTime - firstPressTime2 >= 0.5)
			{
				//firstPressTime2 = currentTime;
				dialogue = cubeNum;
				firstTime = false;
				firstWhite = false;
			}
		}
		secondPressTime = currentTime;
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
}

void drawGroundPlane()
{
	glDisable(GL_BLEND);
	glDisable(GL_CULL_FACE);
	glUseProgram(renderingProgram);

	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	//glEnable(GL_CULL_FACE);
	//glFrontFace(GL_CCW);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glUniform3f(colorLoc, 1.0, 1.0, 0.0); //yellow
	aspect = (float)width / (float)height;
	pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f); // 1.0472 radians = 60 degrees

	vMat = glm::inverse(glm::mat4(cameraVX, cameraVY, cameraVZ, glm::vec4(cameraX, cameraY, cameraZ, 1.0)));
	
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(vMat));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));

	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void makeDecision(bool &firstDraw, bool &secondDraw)
{
	if (isPressedY)
	{
		firstDraw = true;
	}

	if (isPressedN)
	{
		secondDraw = true;
	}
}

void display(GLFWwindow* window, double currentTime) {

	glClear(GL_DEPTH_BUFFER_BIT);
	glClear(GL_COLOR_BUFFER_BIT);


	//glUseProgram(renderingProgram); // tells it to use the program
	//glBindVertexArray(vao[0]);
	
	// get the uniform variables for the MV and projection matrices
	mvLoc = glGetUniformLocation(renderingProgram, "mv_matrix");
	projLoc = glGetUniformLocation(renderingProgram, "proj_matrix");
	colorLoc = glGetUniformLocation(renderingProgram, "color");

	// build perspective matrix
	glfwGetFramebufferSize(window, &width, &height);

//Draw cubes

//drawCube won't work by itself
	if (!titleScreen)
	{
		drawGroundPlane();

		drawCube(0, currentTime);
		drawCube(1, currentTime);
		drawCube(2, currentTime);
		drawCube(3, currentTime);
		drawCube(4, currentTime);
		drawCube(5, currentTime);
	}


	//First call doesn't display for some reason
	drawText("This is sample text", 35.0f, 50.0f, 0.1f, glm::vec3(0.0, 0.0f, 0.0f));

	
	if (titleScreen)
	{
		drawText("TOTAL DRAMA CUBELAND", 65.0f, 400.0f, 0.8f, glm::vec3(1.0, 1.0f, 0.0f)); //
		drawText("Press SPACE to Start", 175.0f, 350.0f, 0.5f, glm::vec3(0.3, 0.7f, 0.9f)); //blue
	}

	if (!titleScreen && !startedGame)
	{
		drawText( "Use WASD to move", 200.0f, 80.0f, 0.5f, glm::vec3(0.3, 0.7f, 0.9f)); //blue
	}
	
	if (firstWhite)
	{
 		//drawText("If the cube turns white, press E to switch cubes or SPACE to talk", 32.0f, 88.0f, 0.4f, glm::vec3(0.3, 0.7f, 0.9f));
		drawText("If the cube turns white, press SPACE to talk", 50.0f, 88.0f, 0.5f, glm::vec3(0.3, 0.7f, 0.9f));
		//firstWhite = false;
	}

	if (noSwitch)
	{
		drawText("You can't switch with a cube you've already been.", 40.0f, 48.0f, 0.5f, glm::vec3(0.3, 0.7f, 0.9f));
	}

	if(active == 0) //red
	{
		switch (dialogue)
		{
		case 1:
			if (decisions[0] == 0)
			{
				drawText("Hi, Red! Remember me? Teal?", 80.0f, 500.0f, 0.5f, glm::vec3(0.0f, 0.5f, 0.5f)); //teal
			}
			if (decisions[0] == 1)
			{
				drawText("I feel so bad for Purple! We're literally like besties.", 50.0f, 500.0f, 0.5f, glm::vec3(0.0f, 0.5f, 0.5f)); //teal
			}
			if (decisions[0] == 2)
			{
				drawText("The way you acted was messed up.", 50.0f, 500.0f, 0.5f, glm::vec3(0.0f, 0.5f, 0.5f)); //teal
				drawText("You better make things right.", 50.0f, 475.0f, 0.5f, glm::vec3(0.0f, 0.5f, 0.5f)); //teal
				decisions[1] = 1;
			}
			break;
		case 2:
			if (decisions[1] == 0)
			{
				if (decisions[0] == 0)
				{
					drawText("*sigh* Hi there", 50.0f, 500.0f, 0.5f, glm::vec3(0.5, 0.3, 0.6)); //purple
				}
				
				if (decisions[0] == 1)
				{
					//Red is kind to Purple
					drawText("*sniffle* Yes, I'm fine *sob*", 50.0f, 500.0f, 0.5f, glm::vec3(0.5, 0.3, 0.6)); //purple
					drawText("I really appreciate you. I just wish some", 50.0f, 475.0f, 0.5f, glm::vec3(0.5, 0.3, 0.6)); //purple
					drawText("of my other friends would check on me.", 50.0f, 450.0f, 0.5f, glm::vec3(0.5, 0.3, 0.6)); //purple
					finishedRound = true;
					afterR1 = true;
				}

				if (decisions[0] == 2)
				{
					//Purple has a beef with Red
					drawText("Fuscia told me that you don't care about me", 50.0f, 500.0f, 0.5f, glm::vec3(0.5, 0.3, 0.6)); //purple
					drawText("*cries violently*", 50.0f, 475.0f, 0.5f, glm::vec3(0.5, 0.3, 0.6)); //purple
				}
			}
		

			if (decisions[1] == 1)
			{
				if (decisions[2] == 0)//firstPass3)
				{
					drawText("Are you here to say you're sorry?", 50.0f, 500.0f, 0.5f, glm::vec3(0.5, 0.3, 0.6)); //purple
					drawText("*single tear* Y or N?", 50.0f, 475.0f, 0.5f, glm::vec3(0.5, 0.3, 0.6)); //purple
				}
				
				makeDecision(draw1b, draw2b);

				if (draw1b && !draw2b)
				{
					//Fuchsia likes Red
					drawText("Thank you. I appreciate it", 50.0f, 500.0f, 0.5f, glm::vec3(0.5, 0.3, 0.6)); //purple
					decisions[2] = 1;
					finishedRound = true;
					afterR1 = true;
				}
				else if (draw2b && !draw1b)
				{
					//Fuschia is wary of Red
					drawText("Never talk to me again.*wails in despair*", 50.0f, 450.0f, 0.5f, glm::vec3(0.5, 0.3, 0.6));
					decisions[2] = 2;
					finishedRound = true;
					afterR1 = true;
				}
			}
			break;
		case 3:
			if (decisions[0] == 0)
			{
				drawText("Hey, Red. I know we're friends and all ", 40.0f, 500.0f, 0.5f, glm::vec3(0.6, 0.0, 0.3)); //fuchsia
				drawText("so could you check in on Purple for me?", 50.0f, 475.0f, 0.5f, glm::vec3(0.6, 0.0, 0.3)); //fuchsia
				drawText("(Press Y for yes and N for no)", 50.0f, 450.0f, 0.5f, glm::vec3(0.6, 0.0, 0.3)); //fuchsia
			}
			makeDecision(draw1, draw2);

			if (draw1 && !draw2)
			{
				//Fuchsia likes Red
				drawText("Thanks! I knew I could count on you!", 50.0f, 450.0f, 0.5f, glm::vec3(0.6, 0.0, 0.3));
				decisions[0] = 1;
			}
			else if (draw2 && !draw1)
			{
				//Fuschia is wary of Red
				drawText("Okay whatever... I guess.", 50.0f, 450.0f, 0.5f, glm::vec3(0.6, 0.0, 0.3));
				decisions[0] = 2;
			}
			break;
		case 4:
			if (decisions[0] == 0)
			{
				drawText("Theres something wrong with Purple.", 50.0f, 500.0f, 0.5f, glm::vec3(0.0, 0.5, 0.0)); //green
				drawText("She's been crying for days.", 50.0f, 470.0f, 0.5f, glm::vec3(0.0, 0.5, 0.0));
			}

			if (decisions[0] == 1)
			{
				drawText("Don't look at me. I'm not consoling her.", 50.0f, 500.0f, 0.5f, glm::vec3(0.0, 0.5, 0.0)); //green
			}

			if (decisions[0] == 2)
			{
				drawText("Good on you for finally giving Purple", 50.0f, 500.0f, 0.5f, glm::vec3(0.0, 0.5, 0.0)); //green
				drawText("something to cry about.", 50.0f, 470.0f, 0.5f, glm::vec3(0.0, 0.5, 0.0));
			}
			break;
		case 5:
			if (decisions[0] == 0)
			{
				drawText("Whats good, Red?", 50.0f, 500.0f, 0.5f, glm::vec3(1.0, 0.5, 0.0)); //orange
			}
			if (decisions[0] == 1 || decisions[0] == 2)
			{
				drawText("Keep me out of this!", 50.0f, 500.0f, 0.5f, glm::vec3(1.0, 0.5, 0.0)); //orange
			}
			break;
		}
	}

	if (active == 1) //teal
	{
		switch (dialogue)
		{
		case 0:
			if (decisions[5] == 0)
			{
				if (decisions[0] == 1 || decisions[2] == 1)
				{
					drawText("I feel like such a great person", 80.0f, 500.0f, 0.5f, glm::vec3(1.0f, 0.0f, 0.0f)); //red
				}
				if (decisions[2] == 2)
				{
					drawText("Sometimes I wonder if I'm morally corrupt,", 80.0f, 500.0f, 0.5f, glm::vec3(1.0f, 0.0f, 0.0f)); //red
					drawText("but then I think... nahhh. Not possible", 80.0f, 475.0f, 0.5f, glm::vec3(1.0f, 0.0f, 0.0f)); //red
				}
			}
			if (decisions[5] == 1)
			{
				drawText("Dang, I can't believe you did that.", 80.0f, 500.0f, 0.5f, glm::vec3(1.0f, 0.0f, 0.0f)); //red
				drawText("Shame on you.", 80.0f, 475.0f, 0.5f, glm::vec3(1.0f, 0.0f, 0.0f)); //red
			}
			break;
		case 2:
			if (decisions[11] == 0 && decisions[10] == 0)
			{
				if (decisions[3] == 0)
				{
					drawText("Teeeeaall!! I missed you!", 50.0f, 500.0f, 0.5f, glm::vec3(0.5, 0.3, 0.6)); //purple
					drawText("I'm so glad you're here!", 50.0f, 475.0f, 0.5f, glm::vec3(0.5, 0.3, 0.6));
					drawText("Are you here to cheer me up? Y or N?", 50.0f, 450.0f, 0.5f, glm::vec3(0.5, 0.3, 0.6));
				}

				makeDecision(draw1a, draw2a);

				if (draw1a && !draw2a)
				{
					drawText("Awwww that's so so sweet of you!", 50.0f, 500.0f, 0.5f, glm::vec3(0.5, 0.3, 0.6));
					drawText("I feel so much better!", 50.0f, 475.0f, 0.5f, glm::vec3(0.5, 0.3, 0.6));
					decisions[3] = 1;
				}
				else if (draw2a && !draw1a)
				{
					drawText("Oh, okay. *silent cry*", 50.0f, 450.0f, 0.5f, glm::vec3(0.5, 0.3, 0.6));
					decisions[3] = 2;
				}
			}
			if (decisions[11] == 1 && decisions[6] == 0) //Fuchsia and Purple fighting over Teal
			{
				if (decisions[12] == 0)
				{
					drawText("Fuchsia and I both like you a lot!", 50.0f, 500.0f, 0.5f, glm::vec3(0.5, 0.3, 0.6));
					drawText("I like you more, so will you choose me?", 50.0f, 475.0f, 0.5f, glm::vec3(0.5, 0.3, 0.6));
					drawText("Y or N?", 50.0f, 450.0f, 0.5f, glm::vec3(0.5, 0.3, 0.6));
				}

				makeDecision(draw1i, draw2i);

				if (draw1i && !draw2i)
				{
					drawText("Yayyy I can keep my self esteem!", 50.0f, 500.0f, 0.5f, glm::vec3(0.5, 0.3, 0.6));
					drawText("I love external validation!", 50.0f, 475.0f, 0.5f, glm::vec3(0.5, 0.3, 0.6));
					decisions[12] = 1;
					finishedRound = true;
					afterR1 = true;
				}
				else if (draw2i && !draw1i)
				{
					drawText("Fine whatever. Live happily ever after with her.", 50.0f, 450.0f, 0.5f, glm::vec3(0.5, 0.3, 0.6));
					drawText("See if I care!!", 50.0f, 475.0f, 0.5f, glm::vec3(0.5, 0.3, 0.6));
					decisions[12] = 2;
					finishedRound = true;
					afterR1 = true;
				}
			}
			if ((decisions[11] == 2 || (decisions[6] == 2 || decisions[10] == 2) && decisions[12] != 2 && decisions[3] != 1)) //Fuchsia says she doesn't want to ask out Teal/ Teal rejects Fuchsia 
			{
				if (decisions[13] == 0)
				{
					drawText("I've been trying to hold back my", 50.0f, 500.0f, 0.5f, glm::vec3(0.5, 0.3, 0.6));
					drawText("feelings for you, but I just can't anymore!", 50.0f, 475.0f, 0.5f, glm::vec3(0.5, 0.3, 0.6));
					drawText("Do you feel the same way? Y or N?", 50.0f, 450.0f, 0.5f, glm::vec3(0.5, 0.3, 0.6));
				}

				makeDecision(draw1j, draw2j);
				if (draw1j && !draw2j)
				{
					drawText("Yayyy I can't wait to spend every single", 50.0f, 500.0f, 0.5f, glm::vec3(0.5, 0.3, 0.6));
					drawText("second with you until one of us dies!", 50.0f, 475.0f, 0.5f, glm::vec3(0.5, 0.3, 0.6));
					decisions[13] = 1;
					finishedRound = true;
					afterR1 = true;
				}
				else if (draw2j && !draw1j)
				{
					drawText("Pfft! Screw you! I can easily find someone", 50.0f, 500.0f, 0.5f, glm::vec3(0.5, 0.3, 0.6));
					drawText("better looking!!", 50.0f, 475.0f, 0.5f, glm::vec3(0.5, 0.3, 0.6));
					decisions[13] = 2;
					finishedRound = true;
					afterR1 = true;
				}				
			}
			if (decisions[6] == 1 && decisions[3] != 1)
				{
					drawText("I'm sure you will have a great life with Fuchsia.", 50.0f, 500.0f, 0.5f, glm::vec3(0.5, 0.3, 0.6));
				}
			break;
		case 3:
			if (decisions[3] == 0 && decisions[23] == 0)
			{
				drawText("How are ya?", 250.0f, 500.0f, 0.5f, glm::vec3(0.6, 0.0, 0.3)); //fuchsia
			}

			if (decisions[3] == 1 || decisions[23] == 1)
			{
				if (decisions[6] == 0 && decisions[23] != 1)
				{
					drawText("You're chill for being there for Purple.", 40.0f, 500.0f, 0.5f, glm::vec3(0.6, 0.0, 0.3)); //fuchsia
					drawText("You know, I've always kind of liked you", 40.0f, 475.0f, 0.5f, glm::vec3(0.6, 0.0, 0.3)); //fuchsia
					drawText("Do you wanna be my romantic life partner? Y or N?", 40.0f, 450.0f, 0.5f, glm::vec3(0.6, 0.0, 0.3)); //fuchsia
				}

				if (decisions[23] == 1 && decisions[6] == 0 && decisions[12] != 1)
				{
					drawText("Do you wanna be my romantic life partner? Y or N?", 40.0f, 500.0f, 0.5f, glm::vec3(0.6, 0.0, 0.3)); //fuchsia
				}

				makeDecision(draw1c, draw2c);

				if (draw1c && !draw2c)
				{
					drawText("Yayyy I'm so happy!", 50.0f, 500.0f, 0.5f, glm::vec3(0.6, 0.0, 0.3));
					decisions[6] = 1;
					finishedRound = true;
					afterR1 = true;
				}
				else if (draw2c && !draw1c)
				{
					drawText("Whatever, you're ugly anyway.", 50.0f, 500.0f, 0.5f, glm::vec3(0.6, 0.0, 0.3));
					drawText("I was asking you as a joke.", 50.0f, 475.0f, 0.5f, glm::vec3(0.6, 0.0, 0.3));
					decisions[6] = 2;
					//finishedRound = true;
					//afterR1 = true;
					decisions[4] = 1;
				}
				if (decisions[12] == 1)
				{
					drawText("I'm happy for you and Purple.", 50.0f, 500.0f, 0.5f, glm::vec3(0.6, 0.0, 0.3));
				}
			}

			if (decisions[3] == 2)
			{
				drawText("You weren't a good friend to Purple, so", 50.0f, 500.0f, 0.5f, glm::vec3(0.6, 0.0, 0.3)); //fuchsia
				drawText("don't expect me to be a good friend to you.", 50.0f, 475.0f, 0.5f, glm::vec3(0.6, 0.0, 0.3)); //fuchsia
				decisions[4] = 1;
			}
			break;
		case 4:
			if (decisions[4] == 0)
			{
				drawText("Oh, it's you.", 50.0f, 500.0f, 0.5f, glm::vec3(0.0, 0.5, 0.0)); //green
			}
			
			if (decisions[4] == 1)
			{
				drawText("Fuchsia is spreading a rumor about you.", 50.0f, 500.0f, 0.5f, glm::vec3(0.0, 0.5, 0.0)); //green
				drawText("She said you asked her out then hit on ", 50.0f, 475.0f, 0.5f, glm::vec3(0.0, 0.5, 0.0)); //green
				drawText("Purple in the same day.", 50.0f, 450.0f, 0.5f, glm::vec3(0.0, 0.5, 0.0)); //green
				decisions[5] = 1;	
				finishedRound = true;
				afterR1 = true;
			}
			break;
		case 5:
			if (decisions[5] == 0)
			{
				drawText("Hi", 250.0f, 500.0f, 0.9f, glm::vec3(1.0, 0.5, 0.0)); //orange
			}
			if (decisions[5] == 1)
			{
				drawText("Sorry, I don't fraternize with players.", 50.0f, 500.0f, 0.5f, glm::vec3(1.0, 0.5, 0.0)); //orange
			}
			break;
		}
	}

	if (active == 2) //purple
	{
		switch (dialogue)
		{
		case 0:
			if (decisions[0] == 1 || decisions[2] == 1)
			{
				drawText("Glad you're feeling better", 80.0f, 500.0f, 0.5f, glm::vec3(1.0f, 0.0f, 0.0f)); //red
			}
			if (decisions[2] == 2)
			{
				drawText("Awkward.....", 80.0f, 500.0f, 0.5f, glm::vec3(1.0f, 0.0f, 0.0f)); //red
			}
			break;
		case 1:
			if (decisions[12] == 1 || decisions[13] == 1)
			{
				drawText("Hiiii, babyyy!", 50.0f, 500.0f, 0.5f, glm::vec3(0.0, 0.5, 0.5)); //teal
			}
			else
			{
				drawText("How are you pal??", 50.0f, 500.0f, 0.5f, glm::vec3(0.0, 0.5, 0.5)); //teal
			}

			break;
		case 3:

			if (decisions[6] == 1 && decisions[15] == 0)
			{
				drawText("Not gonna lie, but you seem a little", 50.0f, 500.0f, 0.5f, glm::vec3(0.6, 0.0, 0.3)); //fuchsia
				drawText("jealous of me and Teal.", 50.0f, 475.0f, 0.5f, glm::vec3(0.6, 0.0, 0.3)); //fuchsia
			}

			else if (decisions[12] == 1)
			{
				drawText("GGs my friend. I don't want Teal to", 50.0f, 500.0f, 0.5f, glm::vec3(0.6, 0.0, 0.3)); //fuchsia
				drawText("come between us.", 50.0f,475.0f, 0.5f, glm::vec3(0.6, 0.0, 0.3)); //fuchsia
			}

			else if (decisions[24] == 2)
			{
				drawText("I didn't mean to make you mad.", 50.0f, 500.0f, 0.5f, glm::vec3(0.6, 0.0, 0.3)); //fuchsia
			}

			else if (decisions[11] == 1)
			{
				drawText("May the best person win.", 50.0f, 500.0f, 0.5f, glm::vec3(0.6, 0.0, 0.3)); //fuchsia
			}

			else
			{
				drawText("Heyyy bestieee", 50.0f, 500.0f, 0.5f, glm::vec3(0.6, 0.0, 0.3)); //fuchsia
			}
			break;
		case 4:
			if (decisions[7] == 0)
			{
				drawText("Can I make a confession to you?", 50.0f, 500.0f, 0.5f, glm::vec3(0.0, 0.5, 0.0)); //green
				drawText("Y or N?", 50.0f, 475.0f, 0.5f, glm::vec3(0.0, 0.5, 0.0)); //green
			}

			makeDecision(draw1d, draw2d);

			if (draw1d && !draw2d)
			{
				drawText("Sooo... I cheated on Orange.", 50.0f, 500.0f, 0.5f, glm::vec3(0.0, 0.5, 0.0));
				drawText("Please keep it a secret. ", 50.0f, 475.0f, 0.5f, glm::vec3(0.0, 0.5, 0.0));
				decisions[7] = 1;
			}
			else if (draw2d && !draw1d)
			{
				drawText("Oh, okay then.", 50.0f, 500.0f, 0.5f, glm::vec3(0.0, 0.5, 0.0));
				decisions[7] = 2;
			}
			break;
		case 5:
			if (decisions[7] == 0)
			{
				drawText("Could you go talk to Green, please? ", 50.0f, 500.0f, 0.5f, glm::vec3(1.0, 0.5, 0.0)); //orange
			}
			else
			{
				if (decisions[8] == 0)
				{
					drawText("Is something suspicious going on?", 50.0f, 500.0f, 0.5f, glm::vec3(1.0, 0.5, 0.0)); //orange
					drawText("Y or N?", 50.0f, 475.0f, 0.5f, glm::vec3(1.0, 0.5, 0.0)); //orange
				}

				makeDecision(draw1e, draw2e);
			
				if (decisions[7] == 1 && (draw1e && !draw2e))
				{
					drawText("I knew it!! *sobs*", 50.0f, 500.0f, 0.5f, glm::vec3(1.0, 0.5, 0.0));
					decisions[8] = 1;
					finishedRound = true;
					afterR1 = true;
				}
				if (decisions[7] == 2 && (draw1e && !draw2e))
				{
					drawText("Did you even ask Green?? Are you trying", 50.0f, 500.0f, 0.5f, glm::vec3(1.0, 0.5, 0.0));
					drawText("to ruin my relationship??", 50.0f, 475, 0.5f, glm::vec3(1.0, 0.5, 0.0));
					decisions[8] = 2;
					finishedRound = true;
					afterR1 = true;
				}
			
				if (decisions[7] == 1 && (draw2e && !draw1e))
				{
					drawText("Okay... I guess I still trust Green... for now", 50.0f, 500.0f, 0.5f, glm::vec3(1.0, 0.5, 0.0));
					decisions[8] = 2;
					finishedRound = true;
					afterR1 = true;
				}

				if (decisions[7] == 2 && (draw2e && !draw1e))
				{
					drawText("Ugh, did you even ask? You're useless.", 50.0f, 500.0f, 0.5f, glm::vec3(1.0, 0.5, 0.0));
					decisions[8] = 2;
					finishedRound = true;
					afterR1 = true;
				}				
			}
			break;
		}
	}

	if (active == 3) //fuchsia
	{
		switch (dialogue)
		{
		case 0:
			drawText("I am not sure we should be seen together...", 40.0f, 500.0f, 0.5f, glm::vec3(1.0f, 0.0f, 0.0f)); //red
			break;
		case 1:
			if (decisions[6] == 0 && decisions[9] == 0)
			{
				drawText("Why, hello there.", 50.0f, 500.0f, 0.5f, glm::vec3(0.0, 0.5, 0.5)); //teal
			}

			if (decisions[6] == 1 && decisions[9] == 0)
			{
				drawText("Can we open our relationship?", 50.0f, 500.0f, 0.5f, glm::vec3(0.0, 0.5, 0.5)); //teal
				drawText("Y or N?", 50.0f, 475.0f, 0.5f, glm::vec3(0.0, 0.5, 0.5)); //teal
			}
			makeDecision(draw1f, draw2f);

			if (draw1f && !draw2f)
			{
				drawText("Yay great! On an unrelated note,", 50.0f, 500.0f, 0.5f, glm::vec3(0.0, 0.5, 0.5));
				drawText("I'm sleeping over with Red tonight.", 50.0f, 475.0f, 0.5f, glm::vec3(0.0, 0.5, 0.5));
				decisions[9] = 1;
				finishedRound = true;
				afterR1 = true;
			}
			else if (draw2f && !draw1f)
			{
				drawText("Well if you're gonna be like that, I'm", 40.0f, 500.0f, 0.5f, glm::vec3(0.0, 0.5, 0.5));
				drawText("not sharing my location with you anymore", 40.0f, 475.0f, 0.5f, glm::vec3(0.0, 0.5, 0.5));
				decisions[9] = 2;
				finishedRound = true;
				afterR1 = true;
			}

			if (decisions[6] == 2)
			{
				drawText("Get lost, you loser!", 40.0f, 500.0f, 0.5f, glm::vec3(0.0, 0.5, 0.5));
			}
			break;
		case 2:
			if (decisions[6] == 0 && decisions[3] != 2)
			{
				if (decisions[10] == 0 && decisions[16] == 0)
				{
					timeCheck1 = currentTime;
					drawText("Hey girlypop! Do you think Teal is cute?", 50.0f, 500.0f, 0.5f, glm::vec3(0.5, 0.3, 0.6)); //purple
					drawText("Y or N?", 250.0f, 475.0f, 0.5f, glm::vec3(0.5, 0.3, 0.6)); //purple
				}

				makeDecision(draw1g, draw2g);

				if (draw1g)
				{
					if (decisions[11] == 0)
					{
						drawText("Oh wait. I didn't know you thought that.", 50.0f, 500.0f, 0.5f, glm::vec3(0.5, 0.3, 0.6));
						drawText("Did you want to ask Teal out? Y or N?", 50.0f, 475.0f, 0.5f, glm::vec3(0.5, 0.3, 0.6));
						decisions[10] = 1;
					}

					if (timeCheck2 - timeCheck1 >= 0.5)
					{
						makeDecision(draw1h, draw2h);
					}

					timeCheck2 = currentTime;

					if (draw1h && !draw2h)// && timeCheck2 - timeCheck1 >= 0.5)
					{
						drawText("Well okayyy I guess we're going to have", 50.0f, 500.0f, 0.5f, glm::vec3(0.5, 0.3, 0.6));
						drawText("to fight over them!", 50.0f, 475.0f, 0.5f, glm::vec3(0.5, 0.3, 0.6));
						decisions[11] = 1;
						decisions[23] = 1;
						finishedRound = true;
						afterR1 = true;
					}
					else if (draw2h && !draw1h)
					{
						drawText("Yayyy!! Teal is all mine!!", 50.0f, 500.0f, 0.5f, glm::vec3(0.5, 0.3, 0.6));
						decisions[11] = 2;
						finishedRound = true;
						afterR1 = true;
					}
				}
				else if (draw2g && !draw1g)
				{
					drawText("Well, I think Teal is a-dor-a-ble!", 40.0f, 500.0f, 0.5f, glm::vec3(0.5, 0.3, 0.6));
					drawText("I think I'm going to ask them out!", 40.0f, 475.0f, 0.5f, glm::vec3(0.5, 0.3, 0.6));
					decisions[10] = 2;
					finishedRound = true;
					afterR1 = true;
				}
			}

			if (decisions[6] == 1)
			{
				drawText("I'm, like, totally happy for you and Teal", 40.0f, 500.0f, 0.5f, glm::vec3(0.5, 0.3, 0.6));
				drawText("or whatever.", 40.0f, 475.0f, 0.5f, glm::vec3(0.5, 0.3, 0.6));
			}

			if (decisions[6] == 2)
			{
				drawText("Teal rejected you??? What an absolute jerk!", 40.0f, 500.0f, 0.5f, glm::vec3(0.5, 0.3, 0.6));
				decisions[16] = 1;
				finishedRound = true;
				afterR1 = true;
			}

			if (decisions[3] == 2)
			{
				if (decisions[24] == 0)
				{
					drawText("Teal is such a fake friend. Do you want", 40.0f, 500.0f, 0.5f, glm::vec3(0.5, 0.3, 0.6));
					drawText("to help me get revenge? Y or N?", 40.0f, 475.0f, 0.5f, glm::vec3(0.5, 0.3, 0.6));
				}

				makeDecision(draw1q, draw2q);

				if (draw1q && !draw2q)
				{
					drawText("Awesome! He won't know what hit him!", 50.0f, 500.0f, 0.5f, glm::vec3(0.5, 0.3, 0.6));
					decisions[24] = 1;
					finishedRound = true;
					afterR1 = true;
				}
				else if (draw2q && !draw1q)
				{
					drawText("Okay, fine. I guess you're a fake", 40.0f, 500.0f, 0.5f, glm::vec3(0.5, 0.3, 0.6));
					drawText("friend too.", 40.0f, 475.0f, 0.5f, glm::vec3(0.5, 0.3, 0.6));
					decisions[24] = 2;
					finishedRound = true;
					afterR1 = true;
				}
			}
			break;
		case 4:
			drawText("Hey, Fuchsia.", 50.0f, 500.0f, 0.5f, glm::vec3(0.0, 0.5, 0.0)); //green
			break;
		case 5:
			drawText("I don't know what's going on.", 50.0f, 500.0f, 0.5f, glm::vec3(1.0, 0.5, 0.0)); //orange
			break;
		}
	}

	if (active == 4) //green
	{
		switch (dialogue)
		{
		case 0:
			if (decisions[19] == 0)
			{
				drawText("You look soooo good today, Green.", 40.0f, 500.0f, 0.5f, glm::vec3(1.0f, 0.0f, 0.0f)); //red
				drawText("Orange wouldn't have to know if we did", 40.0f, 475.0f, 0.5f, glm::vec3(1.0f, 0.0f, 0.0f)); //red
				drawText("anything, so do you want to? Y or N?", 40.0f, 450.0f, 0.5f, glm::vec3(1.0f, 0.0f, 0.0f)); //red
			}

			makeDecision(draw1n, draw2n);

			if (draw1n && !draw2n)
			{
				drawText("I knew you wanted me!", 50.0f, 500.0f, 0.5f, glm::vec3(1.0f, 0.0f, 0.0f));
				drawText("Now come here!", 50.0f, 475.0f, 0.5f, glm::vec3(1.0f, 0.0f, 0.0f));
				decisions[19] = 1;
			}
			else if (draw2n && !draw1n)
			{
				drawText("We did it before, so I don't know", 40.0f, 500.0f, 0.5f, glm::vec3(1.0f, 0.0f, 0.0f));
				drawText("what the hesitation is. If you change", 40.0f, 475.0f, 0.5f, glm::vec3(1.0f, 0.0f, 0.0f));
				drawText("your mind, I'll be waiting for you.", 40.0f, 450.0f, 0.5f, glm::vec3(1.0f, 0.0f, 0.0f));
				decisions[19] = 2;
			}
			break;
		case 1:
			drawText("Hello, Green.", 50.0f, 500.0f, 0.5f, glm::vec3(0.0, 0.5, 0.5)); //teal
			break;
		case 2:
			drawText("Green! Long time no see!", 50.0f, 500.0f, 0.5f, glm::vec3(0.5, 0.3, 0.6)); //purple
			break;
		case 3:
			drawText("Sorry, can't talk now!", 50.0f, 500.0f, 0.5f, glm::vec3(0.6, 0.0, 0.3)); //fuchsia
			break;
		case 5:
			if (decisions[8] == 0)
			{
				if (decisions[19] == 0 && decisions[18] == 0)
				{
					drawText("I can't wait for tonight! Let's talk", 50.0f, 500.0f, 0.5f, glm::vec3(1.0, 0.5, 0.0)); //orange
					drawText("details later!", 50.0f, 475.0f, 0.5f, glm::vec3(1.0, 0.5, 0.0)); //orange
				}

				if (decisions[18] == 0 && decisions[19] != 0)
				{
					drawText("Hey, baby. Are you ready for our", 50.0f, 500.0f, 0.5f, glm::vec3(1.0, 0.5, 0.0)); //orange
					drawText("date night tonight? Y or N?", 50.0f, 475.0f, 0.5f, glm::vec3(1.0, 0.5, 0.0)); //orange
				}

				makeDecision(draw1m, draw2m);

				if (draw1m && !draw2m)
				{
					drawText("Awesome! I am so excited to spend", 50.0f, 500.0f, 0.5f, glm::vec3(1.0, 0.5, 0.0));
					drawText("quality time with my partner!", 50.0f, 475.0f, 0.5f, glm::vec3(1.0, 0.5, 0.0));
					decisions[18] = 1;
					finishedRound = true;
					afterR1 = true;
				}
				else if (draw2m && !draw1m)
				{
					drawText("What? You're busy? *eyeroll*", 40.0f, 500.0f, 0.5f, glm::vec3(1.0, 0.5, 0.0));
					drawText("We've been planning this for", 40.0f, 475.0f, 0.5f, glm::vec3(1.0, 0.5, 0.0));
					drawText("weeks but whatever.", 40.0f, 450.0f, 0.5f, glm::vec3(1.0, 0.5, 0.0));
					decisions[18] = 1;
					finishedRound = true;
					afterR1 = true;
				}
			}

			if (decisions[8] == 1)
			{
				if (decisions[17] == 0)
				{
					drawText("How could you cheat on me?? ", 50.0f, 500.0f, 0.5f, glm::vec3(1.0, 0.5, 0.0)); //orange
					drawText("Do you even still love me?", 50.0f, 475.0f, 0.5f, glm::vec3(1.0, 0.5, 0.0)); //orange
					drawText("Y or N?", 50.0f, 450.0f, 0.5f, glm::vec3(1.0, 0.5, 0.0)); //orange
				}

				makeDecision(draw1k, draw2k);

				if (draw1k && !draw2k)
				{
					drawText("Well, you're not acting like it!", 50.0f, 500.0f, 0.5f, glm::vec3(1.0, 0.5, 0.0));
					drawText("I don't deserve this! We're done!!", 50.0f, 475.0f, 0.5f, glm::vec3(1.0, 0.5, 0.0));
					decisions[17] = 1;
					finishedRound = true;
					afterR1 = true;
				}
				else if (draw2k && !draw1k)
				{
					drawText("Then why are you still here??", 40.0f, 500.0f, 0.5f, glm::vec3(1.0, 0.5, 0.0));
					drawText("Go on. I never want to see your", 40.0f, 475.0f, 0.5f, glm::vec3(1.0, 0.5, 0.0));
					drawText("idiotic face ever again.", 40.0f, 450.0f, 0.5f, glm::vec3(1.0, 0.5, 0.0));
					decisions[17] = 2;
					finishedRound = true;
					afterR1 = true;
				}
			}

			if (decisions[8] == 2)
			{
				if (decisions[20] == 0)
				{
					drawText("Hey, this may be a crazy question,", 50.0f, 500.0f, 0.5f, glm::vec3(1.0, 0.5, 0.0)); //orange
					drawText("but are you doing anything shady", 50.0f, 475.0f, 0.5f, glm::vec3(1.0, 0.5, 0.0)); //orange
					drawText("behind my back? Y or N?", 50.0f, 450.0f, 0.5f, glm::vec3(1.0, 0.5, 0.0)); //orange
				}

				makeDecision(draw1l, draw2l);

				if (draw1l && !draw2l)
				{
					drawText("Thank you for being honest.", 50.0f, 500.0f, 0.5f, glm::vec3(1.0, 0.5, 0.0));
					drawText("Now get out of my sight before I file a", 50.0f, 475.0f, 0.5f, glm::vec3(1.0, 0.5, 0.0));
					drawText("restraining order!", 50.0f, 450.0f, 0.5f, glm::vec3(1.0, 0.5, 0.0));
					decisions[20] = 1;
					finishedRound = true;
					afterR1 = true;
				}
				else if (draw2l && !draw1l)
				{
					drawText("Okay, great! Sorry I ever", 40.0f, 500.0f, 0.5f, glm::vec3(1.0, 0.5, 0.0));
					drawText("doubted you, baby.", 40.0f, 475.0f, 0.5f, glm::vec3(1.0, 0.5, 0.0));
					decisions[20] = 2;
					finishedRound = true;
					afterR1 = true;
				}
			}
			break;
		}
	}

	if (active == 5) //orange
	{
		switch (dialogue)
		{
		case 0:
			if (decisions[8] == 1 || decisions[20] == 1)
			{
				drawText("Better luck in your next relationship!", 40.0f, 500.0f, 0.5f, glm::vec3(1.0f, 0.0f, 0.0f)); //red
				drawText("haha", 40.0f, 475.0f, 0.5f, glm::vec3(1.0f, 0.0f, 0.0f)); //red
			}
			else
			{
				drawText("You're the person Green is dating??", 40.0f, 500.0f, 0.5f, glm::vec3(1.0f, 0.0f, 0.0f)); //red
				drawText("BAHAHAHAHAHA!!", 40.0f, 475.0f, 0.5f, glm::vec3(1.0f, 0.0f, 0.0f)); //red
			}
			break;
		case 1:
			drawText("Umm... hi?", 50.0f, 500.0f, 0.5f, glm::vec3(0.0, 0.5, 0.5)); //teal
			break;
		case 2:
			if (decisions[8] == 1 || decisions[20] == 1)
			{
				drawText("I am so sorry about what happened", 50.0f, 500.0f, 0.5, glm::vec3(0.5, 0.3, 0.6)); //purple
				drawText("with Green.", 50.0f, 475.0f, 0.5, glm::vec3(0.5, 0.3, 0.6)); //purple
			}
			else
			{
				if (decisions[22] == 0)
				{
					drawText("Are you at all suspicious of Green?", 50.0f, 500.0f, 0.5, glm::vec3(0.5, 0.3, 0.6)); //purple
					drawText("Y or N?", 50.0f, 475.0f, 0.5, glm::vec3(0.5, 0.3, 0.6)); //purple
				}

				makeDecision(draw1p, draw2p);

				if (draw1p && !draw2p)
				{
					drawText("Okay, good. I would stay alert", 50.0f, 500.0f, 0.5f, glm::vec3(0.5, 0.3, 0.6));
					drawText("if I were you.", 50.0f, 475.0f, 0.5f, glm::vec3(0.5, 0.3, 0.6));
					decisions[22] = 1;
					finishedRound = true;
					afterR1 = true;
				}
				else if (draw2p && !draw1p)
				{
					drawText("Well, maybe you should be.", 40.0f, 500.0f, 0.5f, glm::vec3(0.5, 0.3, 0.6));
					decisions[22] = 2;
					finishedRound = true;
					afterR1 = true;
				}
			}
			break;
		case 3:
			drawText("Aww hi, Orange.", 50.0f, 500.0f, 0.5f, glm::vec3(0.6, 0.0, 0.3)); //fuchsia
			break;
		case 4:
			if (decisions[8] == 1 && decisions[17] == 0)
			{
				drawText("I didn't do anything! Purple is lying!!", 50.0f, 500.0f, 0.5f, glm::vec3(0.0, 0.5, 0.0)); //green
				finishedRound = true;
				afterR1 = true;
			}
			else if (decisions[17] == 1 || decisions[20] == 1)
			{
				if (decisions[21] == 0)
				{
					drawText("I am so so so so sorryyyyyyy!!", 50.0f, 500.0f, 0.5f, glm::vec3(0.0, 0.5, 0.0)); //green
					drawText("Can we please work this out?? Will you pleaasee", 50.0f, 475.0f, 0.5f, glm::vec3(0.0, 0.5, 0.0)); //green
					drawText("give me another chance? Y or N?", 50.0f, 450.0f, 0.5f, glm::vec3(0.0, 0.5, 0.0)); //green
				}

				makeDecision(draw1o, draw2o);

				if (draw1o && !draw2o)
				{
					drawText("Thank you thank you thank you!!!", 50.0f, 500.0f, 0.5f, glm::vec3(0.0, 0.5, 0.0));
					drawText("You won't regret it! I love you, baby!", 50.0f, 475.0f, 0.5f, glm::vec3(0.0, 0.5, 0.0));
					decisions[21] = 1;
					finishedRound = true;
					afterR1 = true;
				}
				else if (draw2o && !draw1o)
				{
					drawText("I hate you! You're making a huge mistake!!!", 40.0f, 500.0f, 0.5f, glm::vec3(0.0, 0.5, 0.0));
					decisions[21] = 2;
					finishedRound = true;
					afterR1 = true;
				}
			}
			else if (decisions[17] == 2)
			{
				drawText("I'm... sorry...", 40.0f, 500.0f, 0.5f, glm::vec3(0.0, 0.5, 0.0));
				finishedRound = true;
				afterR1 = true;
			}
			else
			{
				drawText("Hey, baby! Good to see you!", 40.0f, 500.0f, 0.5f, glm::vec3(0.0, 0.5, 0.0));
			}
			break;
		}
	}

	if (beenCube[1] && beenCube[2] && beenCube[3] && beenCube[4] && beenCube[5] && afterR1)
	{
		afterR1 = false;
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		drawText("GAME OVER", 150.0f, 530.0f, 0.9f, glm::vec3(0.3, 0.7f, 0.9f));
		drawText("Results of your choices:", 155.0f, 505.0f, 0.5f, glm::vec3(0.3, 0.7f, 0.9f));

		if (decisions[0] == 1)
		{
			drawText("Red and Purple became lifelong frineds", 40.0f, 425.0f, 0.5f, glm::vec3(1.0f, 0.0f, 0.0f)); //Red
			drawText("because of Red's kindness towards Purple.", 40.0f, 400.0f, 0.5f, glm::vec3(1.0f, 0.0f, 0.0f));
		}

		if (decisions[2] == 1)
		{
			drawText("Since Purple forgave Red, they were able", 40.0f, 450.0f, 0.5f, glm::vec3(1.0f, 0.0f, 0.0f)); //Red
			drawText("to create a friendship regardless of Red's", 40.0f, 425.0f, 0.5f, glm::vec3(1.0f, 0.0f, 0.0f));
			drawText("tendency to be mindlessly cruel on occasion.", 40.0f, 400.0f, 0.5f, glm::vec3(1.0f, 0.0f, 0.0f));
		}

		if (decisions[2] == 2)
		{
			drawText("Purple never did forgive Red after they refused", 40.0f, 425.0f, 0.5f, glm::vec3(1.0f, 0.0f, 0.0f)); //Red
			drawText("to apologize. Purple is a master at holding a grudge.", 40.0f, 400.0f, 0.5f, glm::vec3(1.0f, 0.0f, 0.0f));
		}

		if (decisions[12] == 1)
		{
			drawText("After Purple confessed their love for Teal, they", 40.0f, 350.0f, 0.5f, glm::vec3(0.0, 0.5, 0.5)); //Teal
			drawText("got married and lived a boring, conventional life.", 40.0f, 325.0f, 0.5f, glm::vec3(0.0, 0.5, 0.5));
		}

		if (decisions[12] == 2) 
		{
			drawText("Purple was crushed after Teal's rejection, but they", 40.0f, 350.0f, 0.5f, glm::vec3(0.5, 0.3, 0.6)); //Purple
			drawText("recovered and found a new crush within seconds.", 40.0f, 325.0f, 0.5f, glm::vec3(0.5, 0.3, 0.6));
		}

		if (decisions[13] == 1)
		{
			drawText("Teal and Purple's relationship never was balanced.", 40.0f, 350.0f, 0.5f, glm::vec3(0.0, 0.5, 0.5)); //Teal
			drawText("A long, loveless marriage ended in a messy divorce.", 40.0f, 325.0f, 0.5f, glm::vec3(0.0, 0.5, 0.5));
		}

		if (decisions[13] == 2)
		{
			drawText("It was, in fact, easy for Purple to find someone", 40.0f, 350.0f, 0.5f, glm::vec3(0.5, 0.3, 0.6)); //Purple
			drawText("better looking than Teal. She did so in record time.", 40.0f, 325.0f, 0.5f, glm::vec3(0.5, 0.3, 0.6));
		}

		if (decisions[6] == 1 && decisions[9] == 0)
		{
			drawText("Teal and Fuchsia had a good run, but life took them", 40.0f, 350.0f, 0.5f, glm::vec3(0.0, 0.5, 0.5)); //Teal
			drawText("in different directions. They mutally called it quits.", 40.0f, 325.0f, 0.5f, glm::vec3(0.0, 0.5, 0.5));
		}

		if (decisions[5] == 1) 
		{
			drawText("Teal never did overcome the brutal social ", 40.0f, 265.0f, 0.5f, glm::vec3(0.0, 0.5, 0.5)); //Teal
			drawText("punishment enacted by Fuchsia's rumor.", 40.0f, 240.0f, 0.5f, glm::vec3(0.0, 0.5, 0.5));
			drawText("Teal never had another friend.", 40.0f, 215.0f, 0.5f, glm::vec3(0.0, 0.5, 0.5));
		}

		if (decisions[9] == 1)
		{
			drawText("Obviously Teal had an affair with Red, which threw", 40.0f, 275.0f, 0.5f, glm::vec3(0.0, 0.5, 0.5)); //Teal
			drawText("Fuchsia into a fit of rage and led to an ugly breakup.", 40.0f, 250.0f, 0.5f, glm::vec3(0.0, 0.5, 0.5));
		}

		if (decisions[9] == 2)
		{
			drawText("Fuchsia followed Teal and caught him cheating.", 40.0f, 275.0f, 0.5f, glm::vec3(0.0, 0.5, 0.5)); //Teal
			drawText("Needless to say, they broke up promptly.", 40.0f, 250.0f, 0.5f, glm::vec3(0.0, 0.5, 0.5));
		}

		if (decisions[11] == 1 && decisions[13] == 0 && decisions[6] != 1 && decisions[12] == 0) 
		{
			drawText("Fuchsia and Purple decided to fight to the death", 40.0f, 350.0f, 0.5f, glm::vec3(0.5, 0.3, 0.6)); //Purple
			drawText("to see who could be with Teal. Purple won, but her", 40.0f, 325.0f, 0.5f, glm::vec3(0.5, 0.3, 0.6));
			drawText("face was so bloody Teal rejected her anyway.", 40.0f, 300.0f, 0.5f, glm::vec3(0.5, 0.3, 0.6));
		}

		if (decisions[24] == 1)
		{
			drawText("Purple and Fuchsia went on a wild revenge spree", 40.0f, 350.0f, 0.5f, glm::vec3(0.5, 0.3, 0.6)); //Purple
			drawText("after slashing Teal's tires. They made everyone", 40.0f, 325.0f, 0.5f, glm::vec3(0.5, 0.3, 0.6)); //Purple
			drawText("everyone who had every wronged them pay.", 40.0f, 300.0f, 0.5f, glm::vec3(0.5, 0.3, 0.6)); //Purple
		}

		if (decisions[24] == 2)
		{
			drawText("Purple  got arrested while enacting her revenge", 40.0f, 350.0f, 0.5f, glm::vec3(0.5, 0.3, 0.6)); //Purple
			drawText("against Teal. Fuchsia was there to bail her out,", 40.0f, 325.0f, 0.5f, glm::vec3(0.5, 0.3, 0.6)); //Purple
			drawText("but their friendship was never the same after.", 40.0f, 300.0f, 0.5f, glm::vec3(0.5, 0.3, 0.6)); //Purple
		}

		if ((decisions[11] == 2 || decisions[10] == 2) && decisions[13] == 0)
		{
			drawText("The next time Purple saw Teal, they proposed", 40.0f, 360.0f, 0.5f, glm::vec3(0.5, 0.3, 0.6)); //Purple
			drawText("on the spot. Teal was shocked, but that was", 40.0f, 335.0f, 0.5f, glm::vec3(0.5, 0.3, 0.6));
			drawText("just the type of crazy they were looking for.", 40.0f, 310.0f, 0.5f, glm::vec3(0.5, 0.3, 0.6));
		}

		if (decisions[8] == 1 && decisions[17] == 0)
		{
			drawText("Orange confronted Green about their infidelity,", 40.0f, 175.0f, 0.5f, glm::vec3(1.0, 0.5, 0.0)); //Orange
			drawText("and Green failed spectacularly at weaving any", 40.0f, 150.0f, 0.5f, glm::vec3(1.0, 0.5, 0.0));
			drawText("sort of mildly convincing lie. It was over.", 40.0f, 125.0f, 0.5f, glm::vec3(1.0, 0.5, 0.0));
		}

		if (decisions[17] == 1 && decisions[21] == 0)
		{
			drawText("Orange gave up on love after being cheated on", 40.0f, 175.0f, 0.5f, glm::vec3(1.0, 0.5, 0.0)); //Orange
			drawText("by Green. Luckily they had generational wealth", 40.0f, 150.0f, 0.5f, glm::vec3(1.0, 0.5, 0.0));
			drawText("and nepotism to fall back on.", 40.0f, 125.0f, 0.5f, glm::vec3(1.0, 0.5, 0.0));
		}

		if (decisions[17] == 2)
		{
			drawText("Orange never did see Green's idiotic face again.", 40.0f, 175.0f, 0.5f, glm::vec3(1.0, 0.5, 0.0)); //Orange
			drawText("They tweeted what they said and instantly became", 40.0f, 150.0f, 0.5f, glm::vec3(1.0, 0.5, 0.0));
			drawText("internet famous. Orange then became an influencer.", 40.0f, 125.0f, 0.5f, glm::vec3(1.0, 0.5, 0.0));
		}

		if (decisions[8] == 2 && decisions[20] == 0 && decisions[17] != 1)
		{
			drawText("Eventually it came to light that Green was a", 40.0f, 175.0f, 0.5f, glm::vec3(1.0, 0.5, 0.0)); //Orange
			drawText("habitual infidel. Orange finally decided to pick", 40.0f, 150.0f, 0.5f, glm::vec3(1.0, 0.5, 0.0));
			drawText("their standards off the floor and dump Green.", 40.0f, 125.0f, 0.5f, glm::vec3(1.0, 0.5, 0.0));
		}

		if (decisions[20] == 1 && decisions[21] == 0)
		{
			drawText("Green refused to get out of Orange's sight", 40.0f, 175.0f, 0.5f, glm::vec3(1.0, 0.5, 0.0)); //Orange
			drawText("to the point where a restraining order", 40.0f, 150.0f, 0.5f, glm::vec3(1.0, 0.5, 0.0));
			drawText("had to come into the equation.", 40.0f, 125.0f, 0.5f, glm::vec3(1.0, 0.5, 0.0));
		}

		if (decisions[20] == 2)
		{
			drawText("Green kept up their pathetic charade until", 50.0f, 175.0f, 0.5f, glm::vec3(0.0, 0.5, 0.0)); //Green
			drawText("their wedding day when Red exposed their", 50.0f, 150.0f, 0.5f, glm::vec3(0.0, 0.5, 0.0));
			drawText("affair by objecting during the ceremony.", 50.0f, 125.0f, 0.5f, glm::vec3(0.0, 0.5, 0.0));
		}

		if (decisions[21] == 1)//
		{
			drawText("Green and Orange got married. Everything", 40.0f, 175.0f, 0.5f, glm::vec3(0.0, 0.5, 0.0)); //Green
			drawText("was going very well. They were completely in", 40.0f, 150.0f, 0.5f, glm::vec3(0.0, 0.5, 0.0));
			drawText("love... until Orange caught Green in Red's bed.", 40.0f, 125.0f, 0.5f, glm::vec3(0.0, 0.5, 0.0));
		}

		if (decisions[21] == 2 && decisions[17] !=1 && decisions[17] != 2)
		{
			drawText("Orange was not actually making a huge mistake", 40.0f, 175.0f, 0.5f, glm::vec3(0.0, 0.5, 0.0)); //Green
			drawText("by leaving Green. Orange found love with ", 40.0f, 150.0f, 0.5f, glm::vec3(0.0, 0.5, 0.0));
			drawText("someone way better and Green ended up alone.", 40.0f, 125.0f, 0.5f, glm::vec3(0.0, 0.5, 0.0));
		}

		drawText("Press SPACE to exit", 150.0f, 50.0f, 0.5f, glm::vec3(0.3, 0.7f, 0.9f)); //Blue
		if (isPressedSpace)
		{
			glfwDestroyWindow(window);
			glfwTerminate();
			exit(EXIT_SUCCESS);
		}
	}


	if (afterR1)
	{
		drawText("You can now switch places with one cube once.", 50.0f, 88.0f, 0.5f, glm::vec3(0.3, 0.7f, 0.9f));
		drawText("When a cube turns white, press E to switch.", 50.0f, 68.0f, 0.5f, glm::vec3(0.3, 0.7f, 0.9f));
		afterR1 = false;
	}
}

void window_size_callback(GLFWwindow* win, int newWidth, int newHeight) {
	aspect = (float)newWidth / (float)newHeight;
	glViewport(0, 0, newWidth, newHeight);
	pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	stateA = glfwGetKey(window, GLFW_KEY_A);
	stateW = glfwGetKey(window, GLFW_KEY_W);
	stateD = glfwGetKey(window, GLFW_KEY_D);
	stateS = glfwGetKey(window, GLFW_KEY_S);
	stateE = glfwGetKey(window, GLFW_KEY_E);
	stateSpace = glfwGetKey(window, GLFW_KEY_SPACE);
	stateY = glfwGetKey(window, GLFW_KEY_Y);
	stateN = glfwGetKey(window, GLFW_KEY_N);

	//Moving left
	if (stateA == GLFW_PRESS)
	{
		movingLeft = true;
		startedGame = true;
		noSwitch = false;
	}
	if (stateA == GLFW_RELEASE)
	{
		movingLeft = false;
	}
	
	//Moving Right
	if (stateD == GLFW_PRESS)
	{
		movingRight = true;
		startedGame = true;
		noSwitch = false;
	}
	if (stateD == GLFW_RELEASE)
	{
		movingRight = false;
	}

	//Moving Forward
	if (stateW == GLFW_PRESS)
	{
		movingForward = true;
		startedGame = true;
		noSwitch = false;
	}
	if (stateW == GLFW_RELEASE)
	{
		movingForward = false;
	}

	//Moving Back
	if (stateS == GLFW_PRESS)
	{
		movingBack = true;
		startedGame = true;
		noSwitch = false;
	}
	if (stateS == GLFW_RELEASE)
	{
		movingBack = false;
	}

	//E Key
	if (stateE == GLFW_PRESS)
	{
		isPressedE = true;
	}
	if (stateE == GLFW_RELEASE)
	{
		isPressedE = false;
	}

	//Space Key
	if (stateSpace == GLFW_PRESS)
	{
		isPressedSpace = true;
		titleScreen = false;
	}
	if (stateSpace == GLFW_RELEASE)
	{
		isPressedSpace = false;
	}
	
	//Y Key
	if (stateY == GLFW_PRESS)
	{
		isPressedY = true;
	}
	if (stateY == GLFW_RELEASE)
	{
		isPressedY = false;
	}

	//N Key
	if (stateN == GLFW_PRESS)
	{
		isPressedN = true;
	}
	if (stateN == GLFW_RELEASE)
	{
		isPressedN = false;
	}
}

int main(void) {
	if (!glfwInit()) { exit(EXIT_FAILURE); } //initialize glfw library (initialization function)(may return false if it doesn't work')
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); //Tell OpenGL which version we want to work with (4.1)
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);

	//GLFWwindow* window = glfwCreateWindow(600, 600, "Cube Game", glfwGetPrimaryMonitor(), NULL); //Create a window- specify size and title for the title bar
	GLFWwindow* window = glfwCreateWindow(600, 600, "Cube Game", NULL, NULL);
	glfwMakeContextCurrent(window); //Links the window to a special object in Open GL called OpenGL context
	if (glewInit() != GLEW_OK) { exit(EXIT_FAILURE); } 
	glfwSwapInterval(1);

	glfwSetKeyCallback(window, key_callback);

	init(window);

	while (!glfwWindowShouldClose(window)) {
		//glClear(GL_COLOR_BUFFER_BIT);
		display(window, glfwGetTime());
		glfwSwapBuffers(window);
		glfwPollEvents(); //See if any events have arrived - Throw them away in this example
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}