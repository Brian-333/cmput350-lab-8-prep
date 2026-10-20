#ifndef SPRITE_H
#define SPRITE_H

#include "basic_transform.h"
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <array>

class Sprite {  // could derive from sf::Drawable if you're willing to pay for a vptr, but not
                // necessary.
public:
    Sprite(const sf::Texture& texture) : mTexture(&texture) {
        mMinPointTex = {0.f, 0.f};
        mMaxPointTex = {static_cast<float>(texture.getSize().x), static_cast<float>(texture.getSize().y)};
        mOrigin = {0.f, 0.f};
        mTransform = BasicTransform();
        mVertexArray = sf::VertexArray(sf::PrimitiveType::Triangles, 6);
        recomputeVertexArray();
    }

    Sprite(const sf::Texture& texture, const sf::Vector2f& texMinPoint,
           const sf::Vector2f& texMaxPoint) : mTexture(&texture), mMinPointTex(texMinPoint), mMaxPointTex(texMaxPoint) {
        mOrigin = {0.f, 0.f};
        mTransform = BasicTransform();
        mVertexArray = sf::VertexArray(sf::PrimitiveType::Triangles, 6);
        recomputeVertexArray();
    }

    void draw(sf::RenderTarget& target, sf::RenderStates states) const {
        recomputeVertexArray();
        states.texture = mTexture;
        target.draw(mVertexArray, states);
    }

    sf::Vector2f& position() { return mTransform.translation; }
    const sf::Vector2f& position() const { return mTransform.translation; }
    sf::Vector2f& scale() { return mTransform.scale; }
    const sf::Vector2f& scale() const { return mTransform.scale; }
    sf::Vector2f& origin() { return mOrigin; }
    const sf::Vector2f& origin() const { return mOrigin; }

private:
    // It may help you to write this, which is an array of 6 vertices:
    //   - the first 3 correspond to the vertices of the top-left triangle of a "default rectangle"
    //     ranging from [0, 1] in both dimensions.
    //   - the next 3 correspond to the vertices of the bottom-right triangle. of the default
    //     rectangle.
    static constexpr std::array<sf::Vector2f, 6> DEFAULT_VERTICES = {
        {sf::Vector2f(0.f, 0.f), sf::Vector2f(0.f, 1.f), sf::Vector2f(1.f, 0.f), sf::Vector2f(1.f, 0.f), sf::Vector2f(0.f, 1.f), sf::Vector2f(1.f, 1.f)}
    };


    // if mTexture is valid, recomputes the positions and coords of sf::Vertex for each
    // of 6 possible entries in mVertexArray.
    void recomputeVertexArray() const {
        const sf::Vector2f texSize = mMaxPointTex - mMinPointTex;
        for (size_t i = 0; i < 6; ++i) {
            const sf::Vector2f& u = DEFAULT_VERTICES[i];
            mVertexArray[i].texCoords = mMinPointTex + u * texSize;
            mVertexArray[i].position = mTransform.applyPoint(u * texSize - mOrigin);
        }
    }

    const sf::Texture* mTexture;
    mutable sf::VertexArray mVertexArray;  // geometry, uv coords -- mutable means it can be changed
                                           //  even in const methods.
    sf::Vector2f mMinPointTex;             // AABB for subtexture min point along both x, y
    sf::Vector2f mMaxPointTex;             // AABB for subtexture max point along both x, y
    sf::Vector2f mOrigin;  // position in tex coords of the sprite origin, relative to minPoint
    BasicTransform mTransform;
};

#endif
