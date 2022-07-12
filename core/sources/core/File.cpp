#include "File.h"

File::File(EFileType pFileType)
    : fileType(pFileType)
{
    if (fileType == EFileType::SOUND)
    {
        path = "assets/sounds/listen.mp3";
    }
    else if (fileType == EFileType::MODEL)
    {
        path = "assets/padoru/padoru.obj";
    }
    else if (fileType == EFileType::IMAGE)
    {
        path = "assets/padoru/padoru_atlas.png";
    }
}

void File::SetPath( std::string newPath)
{
    if (IsValid(newPath))
    {
        path = newPath;
    }
}

bool File::IsValid(const std::string newPath) 
{
    if(fileType == EFileType::SOUND)
        {
            if (newPath.find(".mp3") != std::string::npos || newPath.find(".wav") != std::string::npos)
            {
                return true;
            }
        }
    else if(fileType == EFileType::MODEL)
        {
            if (newPath.find(".obj") != std::string::npos || newPath.find(".fbx") != std::string::npos)
            {
                return true;
            }
        }
    else if (fileType == EFileType::IMAGE)
    {
        if (newPath.find(".png") != std::string::npos || newPath.find(".jpg") != std::string::npos || newPath.find(".tga") != std::string::npos)
        {
            return true;
        }
    }

    return false;
}
