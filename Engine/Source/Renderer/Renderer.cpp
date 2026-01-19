#include "Renderer.h"

#include <algorithm>
#include <SDL3/SDL_log.h>
#include <SDL3_image/SDL_image.h>

namespace SquareCore
{
    Renderer::Renderer()
        : camera(1920, 1080)
    {
    }

    Renderer::~Renderer()
    {
        TTF_DestroyRendererTextEngine(textEngineRef);
    }

    void Renderer::Init(SDL_Window* window)
    {
        // Initialize the SDL renderer
        rendererRef = SDL_CreateRenderer(window, NULL);
        if (rendererRef == nullptr)
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Error creating SDL renderer: %s\n", SDL_GetError());
        }

        textEngineRef = TTF_CreateRendererTextEngine(rendererRef);
        if (textEngineRef == nullptr)
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Error creating SDL text engine: %s\n", SDL_GetError());
        }

        // Get the initial window size for the base resolution
        SDL_GetRenderOutputSize(rendererRef, &windowWidth, &windowHeight);

        // Update camera viewport to match window size
        camera.SetViewportSize(windowWidth, windowHeight);
    }

    void Renderer::BeginFrame(float deltaTime, EntityManager& entityManager)
    {
        // Initialize the window width and height for scaling purposes
        int newWidth, newHeight;
        SDL_GetRenderOutputSize(rendererRef, &newWidth, &newHeight);

        // Update camera viewport if window size changed
        if (newWidth != windowWidth || newHeight != windowHeight)
        {
            windowWidth = newWidth;
            windowHeight = newHeight;
            uiManagerRef->OnWindowResize(windowWidth, windowHeight, baseWindowWidth, baseWindowHeight);
            camera.SetViewportSize(windowWidth, windowHeight);
        }

        // Clear the render target with a dark blue color
        SDL_SetRenderDrawColor(rendererRef, 0x00, 0x00, 0x1F, 0xFF);
        SDL_RenderClear(rendererRef);

        // Calculate scaling factors based on the current scaling mode
        float globalScaleX, globalScaleY;
        CalculateScalingFactors(globalScaleX, globalScaleY);

        // Get entities copy for rendering (thread-safe)
        std::vector<Entity> entities = entityManager.GetEntitiesCopy();
        std::sort(entities.begin(), entities.end(), [](const Entity& a, const Entity& b) { return a.zIndex < b.zIndex; });

        // Render all entities
        for (const auto& entity : entities)
        {
            if (!entity.visible) continue;
            RenderEntity(entity, globalScaleX, globalScaleY);
        }
    }

    void Renderer::EndFrame()
    {
        SDL_RenderPresent(rendererRef);
    }

    void Renderer::RenderUI()
    {
        std::unordered_map<uint32_t, UIElement*>& elements = uiManagerRef->GetElements();
        
        float scaleX = static_cast<float>(windowWidth) / baseWindowWidth;
        float scaleY = static_cast<float>(windowHeight) / baseWindowHeight;

        for (auto& [id, element] : elements)
        {
            if (!element->visible) continue;
            
            float scaledX = element->x * scaleX;
            float scaledY = element->y * scaleY;
            float scaledWidth = element->width * scaleX;
            float scaledHeight = element->height * scaleY;

            if (element->type == UIElementType::TEXT)
            {
                if (element->text.textObject)
                {
                    TTF_DrawRendererText(element->text.textObject, scaledX, scaledY);
                }
                continue;
            }

            SDL_FRect rect = {scaledX, scaledY, scaledWidth, scaledHeight};
            RGBA color = element->color;
            SDL_Texture* texture = nullptr;

            if (element->type == UIElementType::BUTTON)
            {
                UIButton* button = static_cast<UIButton*>(element);
                
                if (!button->spritePath.empty() && !button->sprite)
                    button->sprite = LoadUITexture(button->spritePath);
                if (!button->hoverSpritePath.empty() && !button->hoverSprite)
                    button->hoverSprite = LoadUITexture(button->hoverSpritePath);
                if (!button->pressedSpritePath.empty() && !button->pressedSprite)
                    button->pressedSprite = LoadUITexture(button->pressedSpritePath);
                
                if (button->isPressed)
                {
                    color = button->pressedColor;
                    texture = button->pressedSprite ? button->pressedSprite : button->sprite;
                }
                else if (button->isHovered)
                {
                    color = button->hoverColor;
                    texture = button->hoverSprite ? button->hoverSprite : button->sprite;
                }
                else
                {
                    texture = button->sprite;
                }
            }
            else if (element->type == UIElementType::RECT)
            {
                UIRect* uiRect = static_cast<UIRect*>(element);
                
                if (!uiRect->spritePath.empty() && !uiRect->sprite)
                    uiRect->sprite = LoadUITexture(uiRect->spritePath);
                
                texture = uiRect->sprite;
            }
            
            if (texture)
            {
                SDL_SetTextureColorMod(texture, color.r, color.g, color.b);
                SDL_SetTextureAlphaMod(texture, color.a);
                SDL_RenderTexture(rendererRef, texture, nullptr, &rect);
            }
            else
            {
                SDL_SetRenderDrawColor(rendererRef, color.r, color.g, color.b, color.a);
                SDL_SetRenderDrawBlendMode(rendererRef, SDL_BLENDMODE_BLEND);
                SDL_RenderFillRect(rendererRef, &rect);
            }

            if (element->border.thickness > 0.0f)
            {
                SDL_SetRenderDrawColor(rendererRef, element->border.color.r, element->border.color.g,
                                       element->border.color.b, element->border.color.a);

                float thickness = element->border.thickness * std::min(scaleX, scaleY);

                SDL_FRect top = {rect.x - thickness, rect.y - thickness, rect.w + (thickness * 2), thickness};
                SDL_FRect bottom = {rect.x - thickness, rect.y + rect.h, rect.w + (thickness * 2), thickness};
                SDL_FRect left = {rect.x - thickness, rect.y, thickness, rect.h};
                SDL_FRect right = {rect.x + rect.w, rect.y, thickness, rect.h};

                SDL_RenderFillRect(rendererRef, &top);
                SDL_RenderFillRect(rendererRef, &bottom);
                SDL_RenderFillRect(rendererRef, &left);
                SDL_RenderFillRect(rendererRef, &right);
            }

            if (element->text.textObject)
            {
                int textWidth, textHeight;
                TTF_GetTextSize(element->text.textObject, &textWidth, &textHeight);

                float textX = scaledX + (scaledWidth / 2.0f) - (static_cast<float>(textWidth) / 2.0f);
                float textY = scaledY + (scaledHeight / 2.0f) - (static_cast<float>(textHeight) / 2.0f);

                TTF_DrawRendererText(element->text.textObject, textX, textY);
            }
        }
    }

    void Renderer::RenderEntity(const Entity& entity, float globalScaleX, float globalScaleY) const
    {
        // Handle spriteless entities
        if (entity.isSpriteless)
        {
            // Calculate scaled dimensions
            float scaledWidth = entity.spritelessWidth * entity.scale.x * globalScaleX;
            float scaledHeight = entity.spritelessHeight * entity.scale.y * globalScaleY;

            // Apply camera transform to get camera-relative position
            Vec2 cameraRelativePos = camera.ApplyCameraTransform(entity.position);

            // Calculate screen position
            float finalXPos, finalYPos;
            if (scalingMode == ScalingMode::PixelBased)
            {
                finalXPos = (cameraRelativePos.x + (static_cast<float>(windowWidth) / 2.0f)) - (scaledWidth / 2.0f);
                finalYPos = (-cameraRelativePos.y + (static_cast<float>(windowHeight) / 2.0f)) - (scaledHeight / 2.0f);
            }
            else
            {
                float scaledXPos = cameraRelativePos.x * globalScaleX;
                float scaledYPos = cameraRelativePos.y * globalScaleY;
                finalXPos = (scaledXPos + (static_cast<float>(windowWidth) / 2.0f)) - (scaledWidth / 2.0f);
                finalYPos = (-scaledYPos + (static_cast<float>(windowHeight) / 2.0f)) - (scaledHeight / 2.0f);
            }

            // Create SDL rectangle
            SDL_FRect rect = {
                finalXPos,
                finalYPos,
                scaledWidth,
                scaledHeight
            };

            // Set draw color with alpha
            SDL_SetRenderDrawColor(rendererRef, entity.spritelessColor.r, entity.spritelessColor.g,
                                   entity.spritelessColor.b, entity.spritelessColor.a);
            SDL_SetRenderDrawBlendMode(rendererRef, SDL_BLENDMODE_BLEND);

            // Draw filled rectangle
            SDL_RenderFillRect(rendererRef, &rect);

            // Draw debug collision box if enabled
            if (debugCollisions && entity.collider.type != ColliderType::NONE)
            {
                SDL_SetRenderDrawColor(rendererRef, 255, 0, 0, 255);
                SDL_RenderRect(rendererRef, &rect);
            }

            return;
        }

        // Handle sprite entities
        if (entity.spriteSheet == nullptr) return;

        float spriteWidth = entity.spriteWidth;

        // Handle switching frames for animated entities
        if (entity.totalFrames > 1)
        {
            spriteWidth = entity.spriteWidth / static_cast<float>(entity.totalFrames);
        }

        // Render the entity sprite to the screen
        SDL_FRect srcRect = {
            (static_cast<float>(entity.currentFrame) * spriteWidth),
            0.0f,
            spriteWidth,
            entity.spriteHeight
        };

        // Apply scaling mode calculations
        float finalSpriteWidth = spriteWidth * entity.scale.x * globalScaleX;
        float finalSpriteHeight = entity.spriteHeight * entity.scale.y * globalScaleY;

        // Apply camera transform to get camera-relative position (includes camera offset and zoom)
        Vec2 cameraRelativePos = camera.ApplyCameraTransform(entity.position);

        // Calculate sprite position with scaling mode consideration
        float finalXPos, finalYPos;
        if (scalingMode == ScalingMode::PixelBased)
        {
            // In pixel-based mode, positions remain constant in screen coordinates
            finalXPos = (cameraRelativePos.x + (static_cast<float>(windowWidth) / 2.0f)) - (finalSpriteWidth / 2.0f);
            finalYPos = (-cameraRelativePos.y + (static_cast<float>(windowHeight) / 2.0f)) - (finalSpriteHeight / 2.0f);
        }
        else
        {
            // In proportional mode, positions scale with the window
            float scaledXPos = cameraRelativePos.x * globalScaleX;
            float scaledYPos = cameraRelativePos.y * globalScaleY;
            finalXPos = (scaledXPos + (static_cast<float>(windowWidth) / 2.0f)) - (finalSpriteWidth / 2.0f);
            finalYPos = (-scaledYPos + (static_cast<float>(windowHeight) / 2.0f)) - (finalSpriteHeight / 2.0f);
        }

        // Set the destination rectangle for sprite rendering
        SDL_FRect dstRect = {
            finalXPos,
            finalYPos,
            finalSpriteWidth,
            finalSpriteHeight
        };

        // Determine flip flags based on entity settings
        SDL_FlipMode flipMode = SDL_FLIP_NONE;
        if (entity.flipX && entity.flipY)
        {
            flipMode = static_cast<SDL_FlipMode>(SDL_FLIP_HORIZONTAL | SDL_FLIP_VERTICAL);
        }
        else if (entity.flipX)
        {
            flipMode = SDL_FLIP_HORIZONTAL;
        }
        else if (entity.flipY)
        {
            flipMode = SDL_FLIP_VERTICAL;
        }
        
        SDL_SetTextureColorMod(entity.spriteSheet, entity.color.r, entity.color.g, entity.color.b);
        SDL_SetTextureAlphaMod(entity.spriteSheet, entity.color.a);
        bool success = SDL_RenderTextureRotated(rendererRef, entity.spriteSheet, &srcRect, &dstRect,
                                                entity.rotation, nullptr, flipMode);

        if (!success)
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Error rendering entity: %s\n", SDL_GetError());
        }

        if (debugCollisions)
        {
            // Calculate collision box dimensions
            float frameWidth = entity.totalFrames > 1
                                   ? (entity.spriteWidth / static_cast<float>(entity.totalFrames))
                                   : entity.spriteWidth;
            float collisionWidth = frameWidth * abs(entity.scale.x);
            float collisionHeight = entity.spriteHeight * abs(entity.scale.y);

            // Calculate world space collision bounds
            Vec2 worldCollisionPos = Vec2(
                entity.position.x - (collisionWidth / 2.0f),
                entity.position.y - (collisionHeight / 2.0f)
            );

            // Apply camera transform
            Vec2 cameraRelativeCollisionPos = camera.ApplyCameraTransform(worldCollisionPos);

            // Convert camera-relative space to screen space
            float screenX, screenY;
            if (scalingMode == ScalingMode::PixelBased)
            {
                screenX = (cameraRelativeCollisionPos.x + (static_cast<float>(windowWidth) / 2.0f));
                screenY = (-cameraRelativeCollisionPos.y + (static_cast<float>(windowHeight) / 2.0f)) - collisionHeight;
            }
            else
            {
                float scaledX = cameraRelativeCollisionPos.x * globalScaleX;
                float scaledY = cameraRelativeCollisionPos.y * globalScaleY;
                screenX = (scaledX + (static_cast<float>(windowWidth) / 2.0f));
                screenY = (-scaledY + (static_cast<float>(windowHeight) / 2.0f)) - (collisionHeight * globalScaleY);
            }

            // Draw debug collision box with correct dimensions
            SDL_SetRenderDrawColor(rendererRef, 255, 0, 0, 255);
            SDL_FRect debugRect = {
                screenX,
                screenY,
                collisionWidth * (scalingMode == ScalingMode::Proportional ? globalScaleX : 1.0f),
                collisionHeight * (scalingMode == ScalingMode::Proportional ? globalScaleY : 1.0f)
            };
            SDL_RenderRect(rendererRef, &debugRect);
        }
    }

    SDL_Texture* Renderer::LoadUITexture(const std::string& path)
    {
        if (path.empty()) return nullptr;
        
        SDL_Surface* surface = IMG_Load(path.c_str());
        if (!surface)
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to load image %s: %s", path.c_str(), SDL_GetError());
            return nullptr;
        }
        
        SDL_Texture* texture = SDL_CreateTextureFromSurface(rendererRef, surface);
        if (!texture)
        {
            return nullptr;
        }
        SDL_DestroySurface(surface);
        
        return texture;
    }

    void Renderer::ToggleScalingMode()
    {
        ScalingMode newMode = (scalingMode == ScalingMode::PixelBased)
                                  ? ScalingMode::Proportional
                                  : ScalingMode::PixelBased;
        scalingMode = newMode;
    }

    void Renderer::CalculateScalingFactors(float& scaleX, float& scaleY) const
    {
        switch (scalingMode)
        {
        case ScalingMode::PixelBased:
            // Constant size - no scaling based on window size
            scaleX = 1.0f;
            scaleY = 1.0f;
            break;
        case ScalingMode::Proportional:
            // Proportional scaling based on window size change
            scaleX = static_cast<float>(windowWidth) / baseWindowWidth;
            scaleY = static_cast<float>(windowHeight) / baseWindowHeight;
            break;
        }
    }

    void Renderer::ToggleDebugCollisions()
    {
        debugCollisions = !debugCollisions;
    }

    void Renderer::Resize(int width, int height)
    {
        windowWidth = width;
        windowHeight = height;
        camera.SetViewportSize(width, height);
    }

    Camera& Renderer::GetCamera()
    {
        return camera;
    }

    Vec2 Renderer::ScreenToWorld(const Vec2& screenPos) const
    {
        float globalScaleX, globalScaleY;
        CalculateScalingFactors(globalScaleX, globalScaleY);

        float scaledX = screenPos.x - (static_cast<float>(windowWidth) / 2.0f);
        float scaledY = (static_cast<float>(windowHeight) / 2.0f) - screenPos.y;

        float cameraRelativeX = scaledX / globalScaleX;
        float cameraRelativeY = scaledY / globalScaleY;

        float zoom = camera.GetZoom();
        Vec2 camPos = camera.GetPosition();

        return Vec2(
            (cameraRelativeX / zoom) + camPos.x,
            (cameraRelativeY / zoom) + camPos.y
        );
    }

    void Renderer::SetUIManager(UIManager* uiManager)
    {
        uiManagerRef = uiManager;
        uiManagerRef->OnWindowResize(windowWidth, windowHeight, baseWindowWidth, baseWindowHeight);
    }
}
