#include "AnitoTracerPCH.h"
#include "Platform/EditorWindow.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

int main()
{
    Vector3 v1(1.0f, 2.0f, 3.0f);
    Vector3 v2(4.0f, 5.0f, 6.0f);

    Vector3 sum = v1 + v2;
    Vector3 crossProduct = v1.Cross(v2);
    float dotProduct = v1.Dot(v2);

    std::cout << "Sum: " << sum.x << ", " << sum.y << ", " << sum.z << std::endl;
    std::cout << "Cross Product: " << crossProduct.x << ", " << crossProduct.y << ", " << crossProduct.z << std::endl;
    std::cout << "Dot Product: " << dotProduct << std::endl;


	Anito::EditorWindow* editorWindow = new Anito::EditorWindow();
	editorWindow->run();

	return 0;
}
