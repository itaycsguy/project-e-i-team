#pragma once
#define _USE_MATH_DEFINES

#include "ImguiMenus.h"
#include "MeshModel.h"
#include "Utils.h"
#include <cmath>
#include <memory>
#include <stdio.h>
#include <string>
#include <sstream>
#include <stdlib.h>
#include <nfd.h>
#include <random>
#include <iostream>

bool showTransformationsWindow = false;
bool showDemoWindow = false;
bool showAnotherWindow = false;
bool showSimpleWindow = true;
bool DL = false;
static int drawLineCounter = 0;
static bool DrawLine = false;

glm::vec4 clearColor = glm::vec4(0.8f, 0.8f, 0.8f, 1.00f);

const glm::vec4& GetClearColor(){
	return clearColor;
}

const char*  getLoadedModels(Scene scene) {
	int length = scene.GetModelCount();
	if (length == 0) {
		return "\0";
	}
	string cStr = "";
	for (size_t i = 0; i < length; i++) {
		cStr += scene.GetModel(i)->GetModelName();
		cStr += '\0';
	}
	int listLength = cStr.length();
	char* comboList = new char[listLength];
	for (size_t i = 0; i < listLength; i++) {
		comboList[i] = cStr.at(i);
	}
	return comboList;
}

glm::mat4x4 getRotateByXTransMat(float teta) {
	return glm::mat4x4(glm::highp_mat4::col_type(1.0f, 0.0f, 0.0f, 0.0f),
					   glm::highp_mat4::col_type(0.0f, cos(teta), (-1)*sin(teta), 0.0f),
					   glm::highp_mat4::col_type(0.0f, sin(teta), cos(teta), 0.0f),
					   glm::highp_mat4::col_type(0.0f, 0.0f, 0.0f, 1.0f));
}

glm::mat4x4 getRotateByYTransMat(float teta) {
	return glm::mat4x4(glm::highp_mat4::col_type(cos(teta), 0.0f, (-1)*sin(teta), 0.0f),
					   glm::highp_mat4::col_type(0.0f, 1.0f, 0.0f, 0.0f),
					   glm::highp_mat4::col_type(sin(teta), 0.0f, cos(teta), 0.0f),
					   glm::highp_mat4::col_type(0.0f, 0.0f, 0.0f, 1.0f));
}

glm::mat4x4 getRotateByZTransMat(float teta) {
	return glm::mat4x4(glm::highp_mat4::col_type(cos(teta), (-1)*sin(teta), 0.0f, 0.0f),
					   glm::highp_mat4::col_type(sin(teta), cos(teta), 0.0f, 0.0f),
					   glm::highp_mat4::col_type(0.0f, 0.0f, 1.0f, 0.0f),
					   glm::highp_mat4::col_type(0.0f, 0.0f, 0.0f, 1.0f));
}

void buildTransformationsWindow(Scene scene) {
	ImGui::Begin("Transformations Window", &showTransformationsWindow);
	ImGui::Text("Transformations window:");
	const char* items = getLoadedModels(scene);
	static int modelIndex = 0;
	ImGui::Combo("Model Name", &modelIndex, items, IM_ARRAYSIZE(items));
	std::shared_ptr<MeshModel> m = scene.GetModel(modelIndex);
	static float fScale = 1.0f, fRotatex = 1.0f, fRotatey = 1.0f, fRotatez = 1.0f;
	ImGui::SliderFloat("Scale Object", &fScale, 1.0f, 100.0f);
	glm::mat4x4 scaling = glm::mat4x4(fScale);
	ImGui::SliderFloat("Rotate By X [-2PI,2PI]", &fRotatex, -2.0f*M_PI, 2.0f*M_PI);
	glm::mat4x4 xRotateMat = getRotateByXTransMat(fRotatex);
	ImGui::SliderFloat("Rotate By Y [-2PI,2PI]", &fRotatey, -2.0f*M_PI, 2.0f*M_PI);
	glm::mat4x4 yRotateMat = getRotateByYTransMat(fRotatey);
	ImGui::SliderFloat("Rotate By Z [-2PI,2PI]", &fRotatez, -2.0f*M_PI, 2.0f*M_PI);
	glm::mat4x4 zRotateMat = getRotateByZTransMat(fRotatez);
	if (m != nullptr) {
		m->SetWorldTransformation(zRotateMat*yRotateMat*xRotateMat*scaling*m->GetWorldTransformation());
	}
	if (ImGui::Button("Dismiss")) {
		showTransformationsWindow = false;
	}
	ImGui::End();
}

void DrawImguiMenus(ImGuiIO& io, Scene& scene){
	// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
	if (showDemoWindow){
		ImGui::ShowDemoWindow(&showDemoWindow);
	}

	// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
	if(showSimpleWindow){
		static float f = 0.0f;
		static int counter = 0;
		ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

		ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
		ImGui::Checkbox("Demo Window", &showDemoWindow);      // Edit bools storing our window open/close state
		ImGui::Checkbox("Another Window", &showAnotherWindow);

		ImGui::Checkbox("Transformations Window", &showTransformationsWindow);

		ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
		ImGui::ColorEdit3("clear color", (float*)&clearColor); // Edit 3 floats representing a color

		if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
			counter++;
		ImGui::SameLine();
		ImGui::Text("counter = %d", counter);
		
		
		if (ImGui::Button("Draw Line")) {
			DrawLine = true;
			showDemoWindow = false;
			showAnotherWindow = false;
			showSimpleWindow = false;
			DL = true;
		}
		
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();
	}

	if (DL){
		ImGui::Begin("Draw Line:", &DL);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
		ImGui::Text("Point1:");
		float p1, q1, p2, q2;
		if (DrawLine == true) {
			ImGui::Text("Mouse pos: (%g, %g)", io.MousePos.x, io.MousePos.y);
			if (ImGui::IsMouseClicked(0) && drawLineCounter == 0){
				p1 = io.MousePos.x;
				q1 = io.MousePos.y;
				printf("point1: (%f, %f)\n", io.MousePos.x, io.MousePos.y);
				drawLineCounter++;
			}
			else if (ImGui::IsMouseClicked(0) && drawLineCounter == 1){
				p2 = io.MousePos.x;
				q2 = io.MousePos.y;
				printf("point2: (%f, %f)\n", io.MousePos.x, io.MousePos.y);
				drawLineCounter++;
			}
			if (drawLineCounter == 1) {
				ImGui::Text("point1: (%g, %g)\n", io.MousePos.x, io.MousePos.y);
			}
			if (drawLineCounter == 2) {
				drawLineCounter = 0;
				DrawLine = false;
				showSimpleWindow = true;
				//DL = false;
			}
		}



		if (ImGui::Button("Cancel")){
			DL = false;
		}
		ImGui::End();
	}





	// 3. Show another simple window.
	if (showAnotherWindow)
	{
		ImGui::Begin("Another Window", &showAnotherWindow);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
		ImGui::Text("Hello from another window!");
		if (ImGui::Button("Dismiss"))
		{
			showAnotherWindow = false;
		}
		ImGui::End();
	}

	// 4. Demonstrate creating a fullscreen menu bar and populating it.
	{
		ImGuiWindowFlags flags = ImGuiWindowFlags_NoFocusOnAppearing;
		if (ImGui::BeginMainMenuBar()){
			if (ImGui::BeginMenu("File")){
				if (ImGui::MenuItem("Load Model...", "CTRL+O")){
					nfdchar_t *outPath = NULL;
					nfdresult_t result = NFD_OpenDialog("obj;png,jpg", NULL, &outPath);
					if (result == NFD_OKAY) {
						MeshModel k = Utils::LoadMeshModel(outPath);
						scene.AddModel(std::make_shared<MeshModel>(Utils::LoadMeshModel(outPath)));
						free(outPath);
					}
					else if (result == NFD_CANCEL) {
					}
					else {
					}

				}
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}
	}

	// 3. Show transformations window.
	if (showTransformationsWindow){
		buildTransformationsWindow(scene);
	}
}
