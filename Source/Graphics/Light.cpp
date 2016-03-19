#include "Light.hpp"

#include "../Foundation/exception.hpp"

#include "AmbientLight.hpp"
#include "DirectionalLight.hpp"
#include "PointLight.hpp"
#include "SpotLight.hpp"

#include <iostream>

#include <fbxsdk.h>

namespace gintonic {

std::shared_ptr<Light> Light::create(const FbxLight* pFbxLight)
{
	const auto lFbxIntensity = static_cast<float>(pFbxLight->Intensity.Get());
	const auto lFbxColor = pFbxLight->Color.Get();
	const vec4f lIntensity(static_cast<float>(lFbxColor[0]), 
		static_cast<float>(lFbxColor[1]), 
		static_cast<float>(lFbxColor[2]), 
		lFbxIntensity);
	vec4f lAttenuation(0.0f, 0.0f, 0.0f, 1.0f);
	std::shared_ptr<Light> lLight;
	
	switch (pFbxLight->DecayType.Get())
	{
		case FbxLight::eNone:
		{
			lAttenuation.x = 0.9f;
			lAttenuation.z = 0.1f;
			break;
		}
		case FbxLight::eLinear:
		{
			lAttenuation.y = 1.0f;
			break;
		}
		case FbxLight::eQuadratic:
		{
			lAttenuation.z = 1.0f;
			break;
		}
		case FbxLight::eCubic:
		{
			throw std::runtime_error("There's not support for cubic falloff.");
		}
		default:
		{
			lAttenuation.z = 1.0f; // quadratic is default
		}
	}
	switch (pFbxLight->LightType.Get())
	{
		case FbxLight::ePoint:
		{
			std::cerr << "\tPoint light\n";
			std::cerr << "\tIntensity: " << lIntensity << '\n';
			std::cerr << "\tAttenuation: " << lAttenuation << '\n';
			lLight = std::make_shared<PointLight>(lIntensity, lAttenuation);
			break;
		}
		case FbxLight::eDirectional:
		{
			std::cerr << "\tDirectional light\n";
			lLight = std::make_shared<DirectionalLight>(lIntensity);
			break;
		}
		case FbxLight::eSpot:
		{
			std::cerr << "\tSpot light\n";
			lLight = std::make_shared<SpotLight>(lIntensity, lAttenuation);
		}
		case FbxLight::eArea:
		{
			std::cerr << "\tArea light is not supported.\n";
			break;
		}
		case FbxLight::eVolume:
		{
			std::cerr << "\tVolume light is not supported.\n";
			break;
		}
		default:
		{
			std::cerr << "\tUnknown light!\n";
			break;
		}
	}
	if (std::strcmp(pFbxLight->GetName(), "") == 0)
	{
		lLight->name = pFbxLight->GetName();
	}
	else
	{
		lLight->name = pFbxLight->GetNode()->GetName();
	}
	return lLight;
}

Light::Light(const vec4f& intensity)
: mIntensity(intensity)
{
	/* Empty on purpose. */
}

void Light::setBrightness(const float brightness)
{
	mIntensity.w = brightness;
}

float Light::getBrightness() const noexcept
{
	return mIntensity.w;
}

std::ostream& operator << (std::ostream& os, const Light* l)
{
	return l->prettyPrint(os);
}

std::ostream& operator << (std::ostream& os, const std::unique_ptr<Light>& l)
{
	return l->prettyPrint(os);
}

std::ostream& operator << (std::ostream& os, const std::shared_ptr<Light>& l)
{
	return l->prettyPrint(os);
}

std::ostream& operator << (std::ostream& os, const Light& l)
{
	return l.prettyPrint(os);
}

std::ostream& Light::prettyPrint(std::ostream& os) const noexcept
{
	return os << "{ (Light) intensity: " << mIntensity << " }";
}

} // namespace gintonic