#ifndef ANIMATION_H
#define ANIMATION_H

#include <SFML/Graphics.hpp>


using namespace sf;

class Animation
{
public:
	std::vector<IntRect> frames, frames_flip;
	float currentFrame, speed;
	bool loop, flip, isPlaying; 
	Sprite sprite;

	Animation()
	{
		currentFrame = 0;
		isPlaying = true;
		flip = false;
		loop = true;
	}

	void tick(float time)
	{
		if (!isPlaying) return;

		currentFrame += speed * time;

		if (currentFrame > frames.size()) {
			currentFrame -= frames.size();
			if (!loop) { isPlaying = false; return; }
		}

		int i = currentFrame;
		sprite.setTextureRect(frames[i]);
		if (flip) sprite.setTextureRect(frames_flip[i]);
	}

};


class AnimationManager
{

public:
	std::string currentAnim;
	std::map<std::string, Animation> animList;

	AnimationManager()
	{}

	~AnimationManager()
	{
		animList.clear();
	}

	void create(std::string name, Texture &texture, int x, int y, int w, int h, int count, float speed, int step = 0, bool Loop = false, bool useFlip=false)
	{
		Animation a;
		a.speed = speed;
		a.loop = Loop;
		a.sprite.setTexture(texture);
		a.sprite.setOrigin(0, h);

		for (int i = 0; i < count; i++)
		{
			a.frames.push_back(IntRect(x + i * (w + step - 1), y, w, h));
			if (useFlip)
			a.frames_flip.push_back(IntRect(x + i + step + w, y, -w, h));
		}
		animList[name] = a;
		currentAnim = name;
	}

	

	void set(std::string name)
	{
		currentAnim = name;
		animList[currentAnim].flip = 0;
	}

	void draw(RenderWindow &window, int x = 0, int y = 0)
	{
		animList[currentAnim].sprite.setPosition(x, y);
		window.draw(animList[currentAnim].sprite);
	}

	void flip(bool b = 1) { animList[currentAnim].flip = b; }

	void tick(float time) { animList[currentAnim].tick(time); }

	void pause() { animList[currentAnim].isPlaying = false; }

	void play() { animList[currentAnim].isPlaying = true; }

	void play(std::string name) { animList[name].isPlaying = true; }

	bool isPlaying() { return animList[currentAnim].isPlaying; }

	float getH() { return animList[currentAnim].frames[0].height; }

	float getW() { return animList[currentAnim].frames[0].width; }

};

#endif ANIMATION_H
