#include <Arduino.h>

#include <Arduboy2.h>
#include <ArduboyPlaytune.h>

// make an instance of arduboy used for many functions
Arduboy2 arduboy;
ArduboyPlaytune tunes(arduboy.audio.enabled);

#define GAME_FPS 60
#define DIR_UP    0
#define DIR_RIGHT 1
#define DIR_DOWN  2
#define DIR_LEFT  3
#define WALK_PERCENT 80
#define SHOT_PERCENT 50
#define TOP_LEFT     0
#define TOP_RIGHT    1
#define TOP_MIDDLE   2
#define BOTTOM_LEFT  3
#define BOTTOM_RIGHT 4
#define ELEMENT_SIZE      10
#define ELEMENT_HALF_SIZE 5
#define SCREEN_MIN_X 1
#define SCREEN_MAX_X 107
#define SCREEN_MIN_Y 1
#define SCREEN_MAX_Y 63
#define PLAYER_POS_X 43
#define PLAYER_POS_Y 57
#define PLAYER_TURN_DELAY 6
#define PLAYER_SHOT_DELAY 10
#define TANK_NORMAL 0
#define TANK_FAST   1
#define TANK_STRONG 2

const uint8_t PROGMEM bullet_frame[] = // 10*10, 20bytes
{
0x00, 0x00, 0x00, 0x00, 0x30, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

const uint8_t PROGMEM player_flag_frames[] = // 10*10, 20bytes
{
  0x00, 0x00, 0x00, 0x30, 0x10, 0x10, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, // up
  0x00, 0x78, 0x00, 0x00, 0x48, 0x78, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // right
  0x00, 0x00, 0x00, 0x32, 0x22, 0x22, 0x32, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // down
  0x00, 0x00, 0x00, 0x00, 0x78, 0x48, 0x00, 0x00, 0x78, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // left
};

const uint8_t PROGMEM enimy_flag_frames[] = // 10*10, 20bytes
{
  0x00, 0x00, 0x00, 0xf0, 0x10, 0x10, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, // up
  0x00, 0x78, 0x48, 0x48, 0x48, 0x78, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // right
  0x00, 0x00, 0x00, 0x3e, 0x22, 0x22, 0x3e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // down
  0x00, 0x00, 0x00, 0x00, 0x78, 0x48, 0x48, 0x48, 0x78, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // left
};

const uint8_t PROGMEM tank_frames[][160] = // 10*10, 20bytes
{
  {
    0xf8, 0x54, 0xf8, 0x11, 0x1f, 0x1f, 0x11, 0xf8, 0x54, 0xf8, 0x01, 0x03, 0x01, 0x03, 0x03, 0x03, 0x03, 0x01, 0x03, 0x01, // up 0
    0xf8, 0xac, 0xf8, 0x11, 0x1f, 0x1f, 0x11, 0xf8, 0xac, 0xf8, 0x01, 0x02, 0x01, 0x03, 0x03, 0x03, 0x03, 0x01, 0x02, 0x01, // up 1
    0x7a, 0xff, 0x85, 0x87, 0x85, 0xff, 0xb5, 0x32, 0x30, 0x78, 0x01, 0x03, 0x02, 0x03, 0x02, 0x03, 0x02, 0x01, 0x00, 0x00, // right 0
    0x7a, 0xfd, 0x87, 0x85, 0x87, 0xfd, 0xb7, 0x32, 0x30, 0x78, 0x01, 0x02, 0x03, 0x02, 0x03, 0x02, 0x03, 0x01, 0x00, 0x00, // right 1
    0x7e, 0xab, 0x7e, 0x23, 0xe3, 0xe3, 0x23, 0x7e, 0xab, 0x7e, 0x00, 0x00, 0x00, 0x02, 0x03, 0x03, 0x02, 0x00, 0x00, 0x00, // down 0
    0x7e, 0xd5, 0x7e, 0x23, 0xe3, 0xe3, 0x23, 0x7e, 0xd5, 0x7e, 0x00, 0x00, 0x00, 0x02, 0x03, 0x03, 0x02, 0x00, 0x00, 0x00, // down 1
    0x78, 0x30, 0x32, 0xb5, 0xff, 0x85, 0x87, 0x85, 0xff, 0x7a, 0x00, 0x00, 0x01, 0x02, 0x03, 0x02, 0x03, 0x02, 0x03, 0x01, // left 0
    0x78, 0x30, 0x32, 0xb7, 0xfd, 0x87, 0x85, 0x87, 0xfd, 0x7a, 0x00, 0x00, 0x01, 0x03, 0x02, 0x03, 0x02, 0x03, 0x02, 0x01, // left 1
  },
  {
    0xfe, 0x55, 0xfe, 0x11, 0x1f, 0x1f, 0x11, 0xfe, 0x55, 0xfe, 0x01, 0x03, 0x01, 0x03, 0x03, 0x03, 0x03, 0x01, 0x03, 0x01, // up 0
    0xfe, 0xab, 0xfe, 0x11, 0x1f, 0x1f, 0x11, 0xfe, 0xab, 0xfe, 0x01, 0x02, 0x01, 0x03, 0x03, 0x03, 0x03, 0x01, 0x02, 0x01, // up 1
    0x7a, 0xff, 0x85, 0x87, 0x85, 0xff, 0xb5, 0xb7, 0xb5, 0x7a, 0x01, 0x03, 0x02, 0x03, 0x02, 0x03, 0x02, 0x03, 0x02, 0x01, // right 0
    0x7a, 0xfd, 0x87, 0x85, 0x87, 0xfd, 0xb7, 0xb5, 0xb7, 0x7a, 0x01, 0x02, 0x03, 0x02, 0x03, 0x02, 0x03, 0x02, 0x03, 0x01, // right 1
    0xfe, 0xab, 0xfe, 0x23, 0xe3, 0xe3, 0x23, 0xfe, 0xab, 0xfe, 0x01, 0x02, 0x01, 0x02, 0x03, 0x03, 0x02, 0x01, 0x02, 0x01, // down 0
    0xfe, 0x55, 0xfe, 0x23, 0xe3, 0xe3, 0x23, 0xfe, 0x55, 0xfe, 0x01, 0x03, 0x01, 0x02, 0x03, 0x03, 0x02, 0x01, 0x03, 0x01, // down 1
    0x7a, 0xb5, 0xb7, 0xb5, 0xff, 0x85, 0x87, 0x85, 0xff, 0x7a, 0x01, 0x02, 0x03, 0x02, 0x03, 0x02, 0x03, 0x02, 0x03, 0x01, // left 0
    0x7a, 0xb7, 0xb5, 0xb7, 0xfd, 0x87, 0x85, 0x87, 0xfd, 0x7a, 0x01, 0x03, 0x02, 0x03, 0x02, 0x03, 0x02, 0x03, 0x02, 0x01, // left 1
  },
  {
    0xff, 0x55, 0xff, 0x10, 0x1f, 0x1f, 0x10, 0xff, 0x55, 0xff, 0x03, 0x03, 0x03, 0x01, 0x01, 0x01, 0x01, 0x03, 0x03, 0x03, // up 0
    0xff, 0xab, 0xff, 0x10, 0x1f, 0x1f, 0x10, 0xff, 0xab, 0xff, 0x03, 0x02, 0x03, 0x01, 0x01, 0x01, 0x01, 0x03, 0x02, 0x03, // up 1
    0x87, 0xff, 0x85, 0x87, 0x85, 0xff, 0xb5, 0xb7, 0xb5, 0xb7, 0x03, 0x03, 0x02, 0x03, 0x02, 0x03, 0x02, 0x03, 0x02, 0x03, // right 0
    0x87, 0xfd, 0x87, 0x85, 0x87, 0xfd, 0xb7, 0xb5, 0xb7, 0xb7, 0x03, 0x02, 0x03, 0x02, 0x03, 0x02, 0x03, 0x02, 0x03, 0x03, // right 1
    0xff, 0xab, 0xff, 0x22, 0xe2, 0xe2, 0x22, 0xff, 0xab, 0xff, 0x03, 0x02, 0x03, 0x00, 0x03, 0x03, 0x00, 0x03, 0x02, 0x03, // down 0
    0xff, 0x55, 0xff, 0x22, 0xe2, 0xe2, 0x22, 0xff, 0x55, 0xff, 0x03, 0x03, 0x03, 0x00, 0x03, 0x03, 0x00, 0x03, 0x03, 0x03, // down 1
    0xb7, 0xb5, 0xb7, 0xb5, 0xff, 0x85, 0x87, 0x85, 0xff, 0x87, 0x03, 0x02, 0x03, 0x02, 0x03, 0x02, 0x03, 0x02, 0x03, 0x03, // left 0
    0xb7, 0xb7, 0xb5, 0xb7, 0xfd, 0x87, 0x85, 0x87, 0xfd, 0x87, 0x03, 0x03, 0x02, 0x03, 0x02, 0x03, 0x02, 0x03, 0x02, 0x03, // left 1
  }
};

const uint8_t PROGMEM explosion_frames[] = // 10*10, 20bytes
{
0x78, 0xce, 0x8b, 0xd1, 0x03, 0x0a, 0xc6, 0x82, 0xec, 0x78, 0x00, 0x01, 0x01, 0x03, 0x02, 0x02, 0x03, 0x01, 0x00, 0x00,
0x38, 0xc6, 0x09, 0x81, 0x43, 0x02, 0x86, 0x01, 0x79, 0xc6, 0x03, 0x02, 0x02, 0x01, 0x01, 0x01, 0x01, 0x01, 0x02, 0x01,
};

struct Explosion {
  public:
    byte delay;
    byte frame;
    int x;
    int y;
    byte active;
};

struct Bullet {
  public:
    byte direction;
    byte flyDelay;
    byte speed;
    int x;
    int y;
    byte active;
};

struct Enimy {
  public:
    boolean walking;
    byte direction;
    byte frame;
    byte speed;
    byte active;
    byte type;
    int x;
    int y;
    Bullet bullets[1];
    Explosion explosion;
};

struct Player {
  public:
    boolean walking;
    byte direction;
    byte frame;
    byte shotDelay;
    byte turnDelay;
    byte type;
    int x;
    int y;
    Bullet bullets[3];
    Explosion explosion;
};

Enimy enimies[4];
Player tank;

/* 
 * explosion
 * 
 */

Explosion initExplosion(byte delay) {
  Explosion obj;
  obj.delay = delay;
  obj.active = false;
  return obj;
}

void enableExplosion(Explosion& obj, int x, int y) {
  obj.x = x;
  obj.y = y;
  obj.frame = 0;
  obj.active = true;
}

void updateExplosion(Explosion& obj) {
  if (arduboy.everyXFrames(12) && obj.active) obj.frame++;
  if (obj.frame > 1) {
    obj.frame = 0;
    obj.active = false;
  }
}

void drawExplosion(Explosion& obj) {
  if (obj.active) arduboy.drawBitmap(obj.x - ELEMENT_HALF_SIZE, obj.y - ELEMENT_HALF_SIZE, &explosion_frames[obj.frame * 20], ELEMENT_SIZE, ELEMENT_SIZE, WHITE);
}

/* 
 * bullet
 * 
 */

Bullet initBullet(byte delay, byte speed) {
  Bullet obj;
  obj.flyDelay = delay;
  obj.speed = speed;
  obj.active = false;
  return obj;
}

void enableBullet(Bullet& obj, int x, int y, byte direction) {
  obj.direction = direction;
  obj.x = x;
  obj.y = y;
  if (direction == DIR_LEFT) {
    obj.x -= ELEMENT_HALF_SIZE;
  } else if (direction == DIR_RIGHT) {
    obj.x += ELEMENT_HALF_SIZE;
  } else if (direction == DIR_UP) {
    obj.y -= ELEMENT_HALF_SIZE;
  } else if (direction == DIR_DOWN) {
    obj.y += ELEMENT_HALF_SIZE;
  }
  obj.active = true;
}

void checkBullet(Bullet& obj, bool player) {
  if (obj.active) {
    for (byte i = 0; i<4; i++) {
      if (enimies[i].active && absSub(obj.x, enimies[i].x) < ELEMENT_HALF_SIZE && absSub(obj.y, enimies[i].y) < ELEMENT_HALF_SIZE) {
        if (player) {
          enimies[i].active = false;
          enableExplosion(enimies[i].explosion, enimies[i].x, enimies[i].y);
          obj.active = false;
        } else {
          obj.active = false;
        }
      }
      if (enimies[i].bullets[0].active && absSub(obj.x, enimies[i].bullets[0].x) < 2 && absSub(obj.y, enimies[i].bullets[0].y) < 2) {
        if (player) {
          enimies[i].bullets[0].active = false;
          obj.active = false;
        }
      }
    }
    if (absSub(obj.x, tank.x) < ELEMENT_HALF_SIZE && absSub(obj.y, tank.y) < ELEMENT_HALF_SIZE) {
      if (player) {
        obj.active = false;
      } else { // game over
        obj.active = false;
      }
    }
    for (byte i = 0; i<3; i++) {
      if (absSub(obj.x, tank.bullets[i].x) < 2 && absSub(obj.y, tank.bullets[i].y) < 2) {
        if (!player) {
          tank.bullets[i].active = false;
          obj.active = false;
        }
      }
    }
  }
}

void updateBullet(Bullet& obj, bool player = false) {
  char vx = 0;
  char vy = 0;

  if (obj.active && arduboy.everyXFrames(obj.flyDelay)) {
    if (obj.direction == DIR_LEFT)
      vx = -obj.speed;
    else if (obj.direction == DIR_RIGHT)
      vx = obj.speed;
    else if (obj.direction == DIR_UP)
      vy = -obj.speed;
    else if (obj.direction == DIR_DOWN)
      vy = obj.speed;

    obj.x += vx;
    if (obj.x >= SCREEN_MAX_X) {
      obj.x = SCREEN_MAX_X;
      obj.active = false;
      if (player) tunes.tone(400, 50);
    } else if (obj.x <= SCREEN_MIN_X) {
      obj.x = SCREEN_MIN_X;
      obj.active = false;
      if (player) tunes.tone(400, 50);
    }

    obj.y += vy;
    if (obj.y >= SCREEN_MAX_Y) {
      obj.y = SCREEN_MAX_Y;
      obj.active = false;
      if (player) tunes.tone(400, 50);
    } else if (obj.y <= SCREEN_MIN_Y) {
      obj.y = SCREEN_MIN_Y;
      obj.active = false;
      if (player) tunes.tone(400, 50);
    }
  }
}

void drawBullet(Bullet& obj) {
  if (obj.active) arduboy.drawBitmap(obj.x - ELEMENT_HALF_SIZE, obj.y - ELEMENT_HALF_SIZE, bullet_frame, ELEMENT_SIZE, ELEMENT_SIZE, WHITE);
}

int absSub(int x, int y) {
  if (x > y) {
    return x - y;
  } else {
    return y - x;
  }
}

bool ablePlace(int x, int y) {
    for (byte i = 0; i<4; i++) {
      if (enimies[i].active && absSub(x, enimies[i].x) < ELEMENT_SIZE && absSub(y, enimies[i].y) < ELEMENT_SIZE) {
        return false;
      }
    }
    if (absSub(x, tank.x) < ELEMENT_SIZE && absSub(y, tank.y) < ELEMENT_SIZE) {
      return false;
    }
    return true;
}

bool ableWalk(int x, int y, byte direction) {
  if (direction == DIR_LEFT) {
    for (byte i = 0; i<4; i++) {
      if (enimies[i].active && (x != enimies[i].x || y != enimies[i].y) && x - enimies[i].x > 0 && x - enimies[i].x <= ELEMENT_SIZE && absSub(y, enimies[i].y) <= ELEMENT_SIZE) {
        return false;
      }
    }
    if ((x != tank.x || y != tank.y) && x - tank.x > 0 && x - tank.x <= ELEMENT_SIZE && absSub(y, tank.y) <= ELEMENT_SIZE) {
      return false;
    }
    return true;
  } else if (direction == DIR_RIGHT) {
    for (byte i = 0; i<4; i++) {
      if (enimies[i].active && (x != enimies[i].x || y != enimies[i].y) && enimies[i].x - x > 0 && enimies[i].x - x <= ELEMENT_SIZE && absSub(y, enimies[i].y) <= ELEMENT_SIZE) {
        return false;
      }
    }
    if ((x != tank.x || y != tank.y) && tank.x - x > 0 && tank.x - x <= ELEMENT_SIZE && absSub(y, tank.y) <= ELEMENT_SIZE) {
      return false;
    }
    return true;
  } else if (direction == DIR_UP) {
    for (byte i = 0; i<4; i++) {
      if (enimies[i].active && (x != enimies[i].x || y != enimies[i].y) && y - enimies[i].y > 0 && y - enimies[i].y <= ELEMENT_SIZE && absSub(x, enimies[i].x) <= ELEMENT_SIZE) {
        return false;
      }
    }
    if ((x != tank.x || y != tank.y) && y - tank.y > 0 && y - tank.y <= ELEMENT_SIZE && absSub(x, tank.x) <= ELEMENT_SIZE) {
      return false;
    }
    return true;
  } else if (direction == DIR_DOWN) {
    for (byte i = 0; i<4; i++) {
      if (enimies[i].active && (x != enimies[i].x || y != enimies[i].y) && enimies[i].y - y > 0 && enimies[i].y - y <= ELEMENT_SIZE && absSub(x, enimies[i].x) <= ELEMENT_SIZE) {
        return false;
      }
    }
    if ((x != tank.x || y != tank.y) && tank.y - y > 0 && tank.y - y <= ELEMENT_SIZE && absSub(x, tank.x) <= ELEMENT_SIZE) {
      return false;
    }
    return true;
  }
}

/* 
 * enimy
 * 
 */
 
Enimy initEnimy(int x, int y, byte speed) {
  Enimy obj;
  obj.direction = random() % 4;
  obj.speed = speed;
  obj.active = false;
  obj.type = TANK_FAST;
  obj.bullets[0] = initBullet(1, 1);
  obj.explosion = initExplosion(1);
  return obj;
}

void enableEnimy(Enimy& obj, int x, int y) {
  obj.x = x;
  obj.y = y;
  obj.active = true;
}

void initEnimies() {
  enimies[0] = initEnimy(SCREEN_MIN_X + ELEMENT_HALF_SIZE, SCREEN_MIN_Y + ELEMENT_HALF_SIZE, 1);
  enimies[1] = initEnimy(SCREEN_MIN_X + ELEMENT_HALF_SIZE, SCREEN_MIN_Y + ELEMENT_HALF_SIZE, 1);
  enimies[2] = initEnimy(SCREEN_MIN_X + ELEMENT_HALF_SIZE, SCREEN_MIN_Y + ELEMENT_HALF_SIZE, 1);
  enimies[3] = initEnimy(SCREEN_MIN_X + ELEMENT_HALF_SIZE, SCREEN_MIN_Y + ELEMENT_HALF_SIZE, 1);
}

void updateEnimies() {
  byte pos;
  for (byte i = 0; i<4; i++) {
    if (!enimies[i].active && !enimies[i].explosion.active) {
      pos = random() % 3; // top_left top_right top_middle
      if (pos == TOP_LEFT && ablePlace(SCREEN_MIN_X + ELEMENT_HALF_SIZE, SCREEN_MIN_Y + ELEMENT_HALF_SIZE)) {
        enableEnimy(enimies[i], SCREEN_MIN_X + ELEMENT_HALF_SIZE, SCREEN_MIN_Y + ELEMENT_HALF_SIZE);
        enimies[i].active = true;
      } else if (pos == TOP_RIGHT && ablePlace(SCREEN_MAX_X - ELEMENT_HALF_SIZE, SCREEN_MIN_Y + ELEMENT_HALF_SIZE)) {
        enableEnimy(enimies[i], SCREEN_MAX_X - ELEMENT_HALF_SIZE, SCREEN_MIN_Y + ELEMENT_HALF_SIZE);
        enimies[i].active = true;
      } else if (pos == TOP_MIDDLE && ablePlace((SCREEN_MIN_X + SCREEN_MAX_X)/2, SCREEN_MIN_Y + ELEMENT_HALF_SIZE)) {
        enableEnimy(enimies[i], (SCREEN_MIN_X + SCREEN_MAX_X)/2, SCREEN_MIN_Y + ELEMENT_HALF_SIZE);
        enimies[i].active = true;
      } else if (pos == BOTTOM_LEFT && ablePlace(SCREEN_MIN_X + ELEMENT_HALF_SIZE, SCREEN_MAX_Y - ELEMENT_HALF_SIZE)) {
        enableEnimy(enimies[i], SCREEN_MIN_X + ELEMENT_HALF_SIZE, SCREEN_MAX_Y - ELEMENT_HALF_SIZE);
        enimies[i].active = true;
      } else if (pos == BOTTOM_RIGHT && ablePlace(SCREEN_MAX_X - ELEMENT_HALF_SIZE, SCREEN_MAX_Y - ELEMENT_HALF_SIZE)) {
        enableEnimy(enimies[i], SCREEN_MAX_X - ELEMENT_HALF_SIZE, SCREEN_MAX_Y - ELEMENT_HALF_SIZE);
        enimies[i].active = true;
      }
    } else {
      updateEnimy(enimies[i]);
    }
    updateBullet(enimies[i].bullets[0]);
    checkBullet(enimies[i].bullets[0], false);
    updateExplosion(enimies[i].explosion);
  }
}

void updateEnimy(Enimy& obj) {
  char vx = 0;
  char vy = 0;

  if (arduboy.everyXFrames(4) && obj.active) {
    bool walk = (random() % 100) < WALK_PERCENT;
    bool shot = (random() % 100) < SHOT_PERCENT;
    byte previousDirection = obj.direction; 
    if (walk) {
      obj.walking = true; // keep direction
    } else {
      obj.direction = random() % 4;
      obj.walking = false;
    }

    if (shot) {
      for (byte i = 0; i<1; i++) {
        if (!obj.bullets[i].active) {
          enableBullet(obj.bullets[i], obj.x, obj.y, obj.direction);
          break;
        }
      }
    }

    if (obj.direction == DIR_LEFT && previousDirection == obj.direction && ableWalk(obj.x, obj.y, obj.direction))
      vx = -obj.speed;
    else if (obj.direction == DIR_RIGHT && previousDirection == obj.direction && ableWalk(obj.x, obj.y, obj.direction))
      vx = obj.speed;
    else if (obj.direction == DIR_UP && previousDirection == obj.direction && ableWalk(obj.x, obj.y, obj.direction))
      vy = -obj.speed;
    else if (obj.direction == DIR_DOWN && previousDirection == obj.direction && ableWalk(obj.x, obj.y, obj.direction))
      vy = obj.speed;

    obj.x += vx;
    if (obj.x >= SCREEN_MAX_X - ELEMENT_HALF_SIZE) {
      obj.x = SCREEN_MAX_X - ELEMENT_HALF_SIZE;
    } else if (obj.x <= SCREEN_MIN_X + ELEMENT_HALF_SIZE) {
      obj.x = SCREEN_MIN_X + ELEMENT_HALF_SIZE;
    }

    obj.y += vy;
    if (obj.y >= SCREEN_MAX_Y - ELEMENT_HALF_SIZE) {
      obj.y = SCREEN_MAX_Y - ELEMENT_HALF_SIZE;
    } else if (obj.y <= SCREEN_MIN_Y + ELEMENT_HALF_SIZE) {
      obj.y = SCREEN_MIN_Y + ELEMENT_HALF_SIZE;
    }
  }

  if (arduboy.everyXFrames(6) && obj.walking) obj.frame++;
  if (obj.frame > 1 ) obj.frame = 0;
}

void drawEnimy(Enimy& obj) {
  if (obj.active) {
    arduboy.drawBitmap(obj.x - ELEMENT_HALF_SIZE, obj.y - ELEMENT_HALF_SIZE, &tank_frames[obj.type][obj.direction * 40 + obj.frame * 20], ELEMENT_SIZE, ELEMENT_SIZE, WHITE);
    arduboy.drawBitmap(obj.x - ELEMENT_HALF_SIZE, obj.y - ELEMENT_HALF_SIZE, &enimy_flag_frames[obj.direction * 20], ELEMENT_SIZE, ELEMENT_SIZE, WHITE);
  }
}

/* 
 * player
 * 
 */

void initPlayer(Player& obj, int x, int y) {
  obj.direction = DIR_UP;
  obj.shotDelay = PLAYER_SHOT_DELAY;
  obj.turnDelay = PLAYER_TURN_DELAY;
  obj.x = x;
  obj.y = y;
  obj.type = TANK_NORMAL;
  obj.bullets[0] = initBullet(1, 2);
  obj.bullets[1] = initBullet(1, 2);
  obj.bullets[2] = initBullet(1, 2);
  obj.explosion = initExplosion(1);
}

void updatePlayer(Player& obj) {
  char vx = 0;
  char vy = 0;
  if (arduboy.everyXFrames(2)) {
    bool left = arduboy.pressed(LEFT_BUTTON);
    bool right = arduboy.pressed(RIGHT_BUTTON);
    bool up = arduboy.pressed(UP_BUTTON);
    bool down = arduboy.pressed(DOWN_BUTTON);

    bool shot = arduboy.pressed(B_BUTTON);

    if (shot && obj.shotDelay == 0) {
      for (byte i = 0; i<3; i++) {
        if (!obj.bullets[i].active) {
          enableBullet(obj.bullets[i], obj.x, obj.y, obj.direction);
          tunes.tone(250, 50);
          obj.shotDelay = PLAYER_SHOT_DELAY;
          break;
        }
      }
    }

    if (obj.shotDelay) obj.shotDelay--;
    if (obj.turnDelay > 0)
      obj.turnDelay--;
    else
      obj.turnDelay = 0;

    obj.walking = up || down || left || right;

    if (left && obj.direction == DIR_LEFT && ableWalk(obj.x, obj.y, obj.direction)) {
      if (obj.turnDelay == 0) vx = -1;
      tunes.tone(35, 50);
    } else if (right && obj.direction == DIR_RIGHT && ableWalk(obj.x, obj.y, obj.direction)) {
      if (obj.turnDelay == 0) vx = 1;
      tunes.tone(35, 50);
    } else if (up && obj.direction == DIR_UP && ableWalk(obj.x, obj.y, obj.direction)) {
      if (obj.turnDelay == 0) vy = -1;
      tunes.tone(35, 50);
    } else if (down && obj.direction == DIR_DOWN && ableWalk(obj.x, obj.y, obj.direction)) {
      if (obj.turnDelay == 0) vy = 1;
      tunes.tone(35, 50);
    }

    obj.x += vx;
    if (obj.x >= SCREEN_MAX_X - ELEMENT_HALF_SIZE) {
      obj.x = SCREEN_MAX_X - ELEMENT_HALF_SIZE;
    } else if (obj.x <= SCREEN_MIN_X + ELEMENT_HALF_SIZE) {
      obj.x = SCREEN_MIN_X + ELEMENT_HALF_SIZE;
    }

    obj.y += vy;
    if (obj.y >= SCREEN_MAX_Y - ELEMENT_HALF_SIZE) {
      obj.y = SCREEN_MAX_Y - ELEMENT_HALF_SIZE;
    } else if (obj.y <= SCREEN_MIN_Y + ELEMENT_HALF_SIZE) {
      obj.y = SCREEN_MIN_Y + ELEMENT_HALF_SIZE;
    }

    if (left && obj.direction != DIR_LEFT) {
      obj.direction = DIR_LEFT;
      obj.turnDelay = PLAYER_TURN_DELAY;
    } else if (right && obj.direction != DIR_RIGHT) {
      obj.direction = DIR_RIGHT;
      obj.turnDelay = PLAYER_TURN_DELAY;
    } else if (up && obj.direction != DIR_UP) {
      obj.direction = DIR_UP;
      obj.turnDelay = PLAYER_TURN_DELAY;
    } else if (down && obj.direction != DIR_DOWN) {
      obj.direction = DIR_DOWN;
      obj.turnDelay = PLAYER_TURN_DELAY;
    }

    if (arduboy.everyXFrames(6) && obj.walking) obj.frame++;
    if (obj.frame > 1 ) obj.frame = 0;
  }
}

void drawPlayer(Player& obj) {
  arduboy.drawBitmap(obj.x - ELEMENT_HALF_SIZE, obj.y - ELEMENT_HALF_SIZE, &tank_frames[obj.type][obj.direction * 40 + obj.frame * 20], ELEMENT_SIZE, ELEMENT_SIZE, WHITE);
  arduboy.drawBitmap(obj.x - ELEMENT_HALF_SIZE, obj.y - ELEMENT_HALF_SIZE, &player_flag_frames[obj.direction * 20], ELEMENT_SIZE, ELEMENT_SIZE, WHITE);
}

void drawBoundary() {
  arduboy.drawLine(SCREEN_MIN_X - 1, SCREEN_MIN_Y - 1, SCREEN_MAX_X, SCREEN_MIN_Y - 1, WHITE);
  arduboy.drawLine(SCREEN_MAX_X, SCREEN_MIN_Y - 1, SCREEN_MAX_X, SCREEN_MAX_Y, WHITE);
  arduboy.drawLine(SCREEN_MIN_X - 1, SCREEN_MAX_Y, SCREEN_MAX_X, SCREEN_MAX_Y, WHITE);
  arduboy.drawLine(SCREEN_MIN_X - 1, SCREEN_MIN_Y - 1, SCREEN_MIN_X - 1, SCREEN_MAX_Y, WHITE);
}

void setup() {
  // put your setup code here, to run once:
  // initiate arduboy instance
  arduboy.begin();
  arduboy.audio.on();

  // here we set the framerate to 15, we do not need to run at
  // default 60 and it saves us battery life
  arduboy.setFrameRate(GAME_FPS);
  arduboy.initRandomSeed();

    // audio setup
  tunes.initChannel(PIN_SPEAKER_1);
#ifndef AB_DEVKIT
  // if not a DevKit
  tunes.initChannel(PIN_SPEAKER_2);
#else
  // if it's a DevKit
  tunes.initChannel(PIN_SPEAKER_1); // use the same pin for both channels
  tunes.toneMutesScore(true);       // mute the score when a tone is sounding
#endif
  
  initPlayer(tank, PLAYER_POS_X, PLAYER_POS_Y);
  initEnimies();
}

void loop() {
  // put your main code here, to run repeatedly:
  int load = 0;
  load = arduboy.cpuLoad();
  
  if (!(arduboy.nextFrame()))
    return;

  // first we clear our screen to black
  arduboy.clear();

  updatePlayer(tank);
  drawPlayer(tank);
  updateBullet(tank.bullets[0], true);
  updateBullet(tank.bullets[1], true);
  updateBullet(tank.bullets[2], true);
  checkBullet(tank.bullets[0], true);
  checkBullet(tank.bullets[1], true);
  checkBullet(tank.bullets[2], true);
  updateExplosion(tank.explosion);
  drawBullet(tank.bullets[0]);
  drawBullet(tank.bullets[1]);
  drawBullet(tank.bullets[2]);
  drawExplosion(tank.explosion);
  
  updateEnimies();
  drawEnimy(enimies[0]);
  drawEnimy(enimies[1]);
  drawEnimy(enimies[2]);
  drawEnimy(enimies[3]);
  drawBullet(enimies[0].bullets[0]);
  drawBullet(enimies[1].bullets[0]);
  drawBullet(enimies[2].bullets[0]);
  drawBullet(enimies[3].bullets[0]);
  drawExplosion(enimies[0].explosion);
  drawExplosion(enimies[1].explosion);
  drawExplosion(enimies[2].explosion);
  drawExplosion(enimies[3].explosion);

  drawBoundary();

//  arduboy.setCursor(0, 48);
//  arduboy.print(String(tank.x));
//  arduboy.setCursor(20, 48);
//  arduboy.print(String(tank.y));
//  arduboy.setCursor(0, 48);
//  arduboy.print(String(load));
//  arduboy.setCursor(16, 48);
//  arduboy.print(String(random()));

  // then we finaly we tell the arduboy to display what we just wrote to the display
  arduboy.display();
}
