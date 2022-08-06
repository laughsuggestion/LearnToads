
#pragma once

// This file is auto-generated -- do not edit it manually!

#include "LTAsset.h"

namespace Content {
class Models {
private: 
    static LTModel* m_Cube; 
public: 

    static inline uint32_t GetCubeID();
    static LTModel* GetCube();
    static LTModel* GetCubeNoLoad();
}; // class Models 

class FragmentShaders {
private: 
    static LTShader* m_Simple; 
public: 

    static inline uint32_t GetSimpleID();
    static LTShader* GetSimple();
    static LTShader* GetSimpleNoLoad();
}; // class FragmentShaders 

class VertexShaders {
private: 
    static LTShader* m_Simple; 
public: 

    static inline uint32_t GetSimpleID();
    static LTShader* GetSimple();
    static LTShader* GetSimpleNoLoad();
}; // class VertexShaders 

} // namespace Content

