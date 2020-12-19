#include "Scene.h"


const LightSource * Scene::CreateLightSource ( DirectX::XMFLOAT3 position, float intencity ) {
    auto& new_light_source = lightSources[numLightSources++];
    new_light_source.position = position;
    new_light_source.intencity = intencity;

    return &new_light_source;
}

void Scene::SolveLighting ( ) {
    
}
