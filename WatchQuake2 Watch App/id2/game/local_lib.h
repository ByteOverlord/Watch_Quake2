//
//  local_lib.h
//  WatchQuake2
//
//  Created by ByteOverlord on 26.11.2022.
//

#ifndef local_lib_h
#define local_lib_h

typedef enum
{
    WEAPON_READY,
    WEAPON_ACTIVATING,
    WEAPON_DROPPING,
    WEAPON_FIRING
} weaponstate_t;

typedef struct gitem_s
{
    char        *classname;    // spawning name
    qboolean    (*pickup)(struct edict_s *ent, struct edict_s *other);
    void        (*use)(struct edict_s *ent, struct gitem_s *item);
    void        (*drop)(struct edict_s *ent, struct gitem_s *item);
    void        (*weaponthink)(struct edict_s *ent);
    char        *pickup_sound;
    char        *world_model;
    int            world_model_flags;
    char        *view_model;

    // client side info
    char        *icon;
    char        *pickup_name;    // for printing on pickup
    int            count_width;        // number of digits to display by icon

    int            quantity;        // for ammo how much, for weapons how much is used per shot
    char        *ammo;            // for weapons
    int            flags;            // IT_* flags

    int            weapmodel;        // weapon model index (for weapons)

    void        *info;
    int            tag;

    char        *precaches;        // string of all models, sounds, and images this item will use
} gitem_t;

typedef struct
{
    // fixed data
    vec3_t        start_origin;
    vec3_t        start_angles;
    vec3_t        end_origin;
    vec3_t        end_angles;

    int            sound_start;
    int            sound_middle;
    int            sound_end;

    float        accel;
    float        speed;
    float        decel;
    float        distance;

    float        wait;

    // state data
    int            state;
    vec3_t        dir;
    float        current_speed;
    float        move_speed;
    float        next_speed;
    float        remaining_distance;
    float        decel_distance;
    void        (*endfunc)(edict_t *);
} moveinfo_t;


typedef struct
{
    void    (*aifunc)(edict_t *self, float dist);
    float    dist;
    void    (*thinkfunc)(edict_t *self);
} mframe_t;

typedef struct
{
    int            firstframe;
    int            lastframe;
    mframe_t    *frame;
    void        (*endfunc)(edict_t *self);
} mmove_t;

typedef struct
{
    mmove_t        *currentmove;
    int            aiflags;
    int            nextframe;
    float        scale;

    void        (*stand)(edict_t *self);
    void        (*idle)(edict_t *self);
    void        (*search)(edict_t *self);
    void        (*walk)(edict_t *self);
    void        (*run)(edict_t *self);
    void        (*dodge)(edict_t *self, edict_t *other, float eta);
    void        (*attack)(edict_t *self);
    void        (*melee)(edict_t *self);
    void        (*sight)(edict_t *self, edict_t *other);
    qboolean    (*checkattack)(edict_t *self);

    float        pausetime;
    float        attack_finished;

    vec3_t        saved_goal;
    float        search_time;
    float        trail_time;
    vec3_t        last_sighting;
    int            attack_state;
    int            lefty;
    float        idle_time;
    int            linkcount;

    int            power_armor_type;
    int            power_armor_power;
} monsterinfo_t;

typedef struct
{
    char        userinfo[MAX_INFO_STRING];
    char        netname[16];
    int            hand;

    qboolean    connected;            // a loadgame will leave valid entities that
                                    // just don't have a connection yet

    // values saved and restored from edicts when changing levels
    int            health;
    int            max_health;
    int            savedFlags;

    int            selected_item;
    int            inventory[MAX_ITEMS];

    // ammo capacities
    int            max_bullets;
    int            max_shells;
    int            max_rockets;
    int            max_grenades;
    int            max_cells;
    int            max_slugs;

    gitem_t        *weapon;
    gitem_t        *lastweapon;

    int            power_cubes;    // used for tracking the cubes in coop games
    int            score;            // for calculating total unit score in coop games

    int            game_helpchanged;
    int            helpchanged;

    qboolean    spectator;            // client is a spectator
} client_persistant_t;

// client data that stays across deathmatch respawns
typedef struct
{
    client_persistant_t    coop_respawn;    // what to set client->pers to on a respawn
    int            enterframe;            // level.framenum the client entered the game
    int            score;                // frags, etc
    vec3_t        cmd_angles;            // angles sent over in the last command

    qboolean    spectator;            // client is a spectator
} client_respawn_t;

#ifdef GAME_INCLUDE
// this structure is cleared on each PutClientInServer(),
// except for 'client->pers'
struct gclient_s
{
    // known to server
    player_state_t    ps;                // communicated by server to clients
    int                ping;

    // private to game
    client_persistant_t    pers;
    client_respawn_t    resp;
    pmove_state_t        old_pmove;    // for detecting out-of-pmove changes

    qboolean    showscores;            // set layout stat
    qboolean    showinventory;        // set layout stat
    qboolean    showhelp;
    qboolean    showhelpicon;

    int            ammo_index;

    int            buttons;
    int            oldbuttons;
    int            latched_buttons;

    qboolean    weapon_thunk;

    gitem_t        *newweapon;

    // sum up damage over an entire frame, so
    // shotgun blasts give a single big kick
    int            damage_armor;        // damage absorbed by armor
    int            damage_parmor;        // damage absorbed by power armor
    int            damage_blood;        // damage taken out of health
    int            damage_knockback;    // impact damage
    vec3_t        damage_from;        // origin for vector calculation

    float        killer_yaw;            // when dead, look at killer

    weaponstate_t    weaponstate;
    vec3_t        kick_angles;    // weapon kicks
    vec3_t        kick_origin;
    float        v_dmg_roll, v_dmg_pitch, v_dmg_time;    // damage kicks
    float        fall_time, fall_value;        // for view drop on fall
    float        damage_alpha;
    float        bonus_alpha;
    vec3_t        damage_blend;
    vec3_t        v_angle;            // aiming direction
    float        bobtime;            // so off-ground doesn't change it
    vec3_t        oldviewangles;
    vec3_t        oldvelocity;

    float        next_drown_time;
    int            old_waterlevel;
    int            breather_sound;

    int            machinegun_shots;    // for weapon raising

    // animation vars
    int            anim_end;
    int            anim_priority;
    qboolean    anim_duck;
    qboolean    anim_run;

    // powerup timers
    float        quad_framenum;
    float        invincible_framenum;
    float        breather_framenum;
    float        enviro_framenum;

    qboolean    grenade_blew_up;
    float        grenade_time;
    int            silencer_shots;
    int            weapon_sound;

    float        pickup_msg_time;

    float        flood_locktill;        // locked from talking
    float        flood_when[10];        // when messages were said
    int            flood_whenhead;        // head pointer for when said

    float        respawn_time;        // can respawn when time > this

    edict_t        *chase_target;        // player we are chasing
    qboolean    update_chase;        // need to update chase info?
};

struct edict_s
{
    entity_state_t    s;
    struct gclient_s    *client;    // NULL if not a player
                                    // the server expects the first part
                                    // of gclient_s to be a player_state_t
                                    // but the rest of it is opaque

    qboolean    inuse;
    int            linkcount;

    // FIXME: move these fields to a server private sv_entity_t
    link_t        area;                // linked to a division node or leaf
    
    int            num_clusters;        // if -1, use headnode instead
    int            clusternums[MAX_ENT_CLUSTERS];
    int            headnode;            // unused if num_clusters != -1
    int            areanum, areanum2;

    //================================

    int            svflags;
    vec3_t        mins, maxs;
    vec3_t        absmin, absmax, size;
    solid_t        solid;
    int            clipmask;
    edict_t        *owner;


    // DO NOT MODIFY ANYTHING ABOVE THIS, THE SERVER
    // EXPECTS THE FIELDS IN THAT ORDER!

    //================================
    int            movetype;
    int            flags;

    char        *model;
    float        freetime;            // sv.time when the object was freed
    
    //
    // only used locally in game, not by server
    //
    char        *message;
    char        *classname;
    int            spawnflags;

    float        timestamp;

    float        angle;            // set in qe3, -1 = up, -2 = down
    char        *target;
    char        *targetname;
    char        *killtarget;
    char        *team;
    char        *pathtarget;
    char        *deathtarget;
    char        *combattarget;
    edict_t        *target_ent;

    float        speed, accel, decel;
    vec3_t        movedir;
    vec3_t        pos1, pos2;

    vec3_t        velocity;
    vec3_t        avelocity;
    int            mass;
    float        air_finished;
    float        gravity;        // per entity gravity multiplier (1.0 is normal)
                                // use for lowgrav artifact, flares

    edict_t        *goalentity;
    edict_t        *movetarget;
    float        yaw_speed;
    float        ideal_yaw;

    float        nextthink;
    void        (*prethink) (edict_t *ent);
    void        (*think)(edict_t *self);
    void        (*blocked)(edict_t *self, edict_t *other);    //move to moveinfo?
    void        (*touch)(edict_t *self, edict_t *other, cplane_t *plane, csurface_t *surf);
    void        (*use)(edict_t *self, edict_t *other, edict_t *activator);
    void        (*pain)(edict_t *self, edict_t *other, float kick, int damage);
    void        (*die)(edict_t *self, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point);

    float        touch_debounce_time;        // are all these legit?  do we need more/less of them?
    float        pain_debounce_time;
    float        damage_debounce_time;
    float        fly_sound_debounce_time;    //move to clientinfo
    float        last_move_time;

    int            health;
    int            max_health;
    int            gib_health;
    int            deadflag;
    qboolean    show_hostile;

    float        powerarmor_time;

    char        *map;            // target_changelevel

    int            viewheight;        // height above origin where eyesight is determined
    int            takedamage;
    int            dmg;
    int            radius_dmg;
    float        dmg_radius;
    int            sounds;            //make this a spawntemp var?
    int            count;

    edict_t        *chain;
    edict_t        *enemy;
    edict_t        *oldenemy;
    edict_t        *activator;
    edict_t        *groundentity;
    int            groundentity_linkcount;
    edict_t        *teamchain;
    edict_t        *teammaster;

    edict_t        *mynoise;        // can go in client only
    edict_t        *mynoise2;

    int            noise_index;
    int            noise_index2;
    float        volume;
    float        attenuation;

    // timing variables
    float        wait;
    float        delay;            // before firing targets
    float        random;

    float        teleport_time;

    int            watertype;
    int            waterlevel;

    vec3_t        move_origin;
    vec3_t        move_angles;

    // move this to clientinfo?
    int            light_level;

    int            style;            // also used as areaportal number

    gitem_t        *item;            // for bonus items

    // common data blocks
    moveinfo_t        moveinfo;
    monsterinfo_t    monsterinfo;
};

#endif // GAME_INCLUDE

#endif /* local_lib_h */
