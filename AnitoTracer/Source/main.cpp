#include "AnitoTracerPCH.h"
#include "Platform/EditorWindow.h"

int main()
{
	Anito::EditorWindow* editorWindow = new Anito::EditorWindow();

	while (editorWindow->running())
	{
		editorWindow->broadcast();
	}
	return 0;
}
