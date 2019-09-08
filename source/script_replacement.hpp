#include <switch_min.h>

#include <stdint.h>

#include "useful/const_value_table.h"
#include "useful/crc32.h"
#include "useful/raygun_printer.h"
#include "useful/useful.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
//#include <switch/result.h>
//#include <stdlib.h>
//#include <switch/runtime/devices/socket.h>
//#include <unistd.h>
#include <cerrno>

#include "saltysd/nn_ro.h"

#include "acmd_wrapper.h"

using namespace lib;
using namespace app::sv_animcmd;
using namespace app::lua_bind;
namespace app {
    namespace utility {
        extern u64 get_kind(u64) asm("_ZN3app7utility8get_kindEPKNS_26BattleObjectModuleAccessorE") LINKABLE;
    }
}
void sv_replace_status_func(u64 l2c_agentbase, int status_kind, u64 key,
                            void* func);

u64 shine_replace(L2CAgent* l2c_agent, void* variadic);
u64 ivy_upsmash(L2CAgent* l2c_agent, void* variadic);
u64 squirtle_utilt(L2CAgent* l2c_agent, void* variadic);
u64 end_shieldbreakfly_replace(u64 l2c_fighter, u64 l2c_agent);

u64 suicide_bomb_acmd_game = 0;

void replace_scripts(L2CAgent* l2c_agent, u8 category, int kind) {
    // fighter
    if (category == BATTLE_OBJECT_CATEGORY_FIGHTER) {
        // fox
        if (kind == FIGHTER_KIND_FOX) {
            l2c_agent->sv_set_function_hash(&shine_replace,
                                            hash40("game_speciallwstart"));
            l2c_agent->sv_set_function_hash(&shine_replace,
                                            hash40("game_specialairlwstart"));
        }

        // ivysaur
        if (kind == FIGHTER_KIND_PFUSHIGISOU) {
            l2c_agent->sv_set_function_hash(&ivy_upsmash,
                                            hash40("game_attackhi4"));
        }

        // squirtle
        if (kind == FIGHTER_KIND_PZENIGAME) {
            l2c_agent->sv_set_function_hash(&squirtle_utilt,
                                            hash40("game_attackhi3"));
            l2c_agent->sv_set_function_hash(
                (u64(*)(L2CAgent*, void*))suicide_bomb_acmd_game,
                hash40("game_attacks3"));
        }

        // peach
        if (kind == FIGHTER_KIND_PEACH) {
            sv_replace_status_func((u64)l2c_agent,
                                   FIGHTER_STATUS_KIND_SHIELD_BREAK_FLY,
                                   LUA_SCRIPT_STATUS_FUNC_STATUS_END,
                                   (void*)&end_shieldbreakfly_replace);
        }
    }
}

// with l2cvalue* in x8
u64 end_shieldbreakfly_replace(u64 l2c_fighter, u64 l2c_agent) {
    L2CValue* l2c_ret;
    asm("mov %x0, x8" : : "r"(l2c_ret) : "x8");

    u64 lua_state = LOAD64(l2c_fighter + 8);
    u64 module_accessor = LOAD64(LOAD64(lua_state - 8) + 416LL);
    void (*is_enable_passive)(u64) = (void (*)(u64))SaltySDCore_FindSymbol(
        "_ZN7lua2cpp16L2CFighterCommon17is_enable_passiveEv");
    L2CValue passive_enabled;
    asm("mov x8, %x0" : : "r"(&passive_enabled));
    is_enable_passive(l2c_fighter);

    if (passive_enabled.raw)
        print_string(module_accessor, "true");
    else
        print_string(module_accessor, "false");

    l2c_ret->type = L2C_integer;
    l2c_ret->raw = 0;
    return 0;
}

// AnimCMD replacement functions
u64 shine_replace(L2CAgent* l2c_agent, void* variadic) {
    ACMD acmd = ACMD(l2c_agent);

    acmd.frame(1);
    if (acmd.is_excute()) {
        acmd.ATTACK(/*ID*/ 0, /*Part*/ 0, /*Bone*/ hash40("top"), /*Damage*/ 10.0, /*Angle*/ 10, /*KBG*/ 32, /*FKB*/ 0, /*BKB*/ 66, /*Size*/ 7.5, /*X*/ 0.0, /*Y*/ 6.5, /*Z*/ 0.0, /*Hitlag*/ 1.0, /*SDI*/ 1.0, /*Clang/Rebound*/ ATTACK_SETOFF_KIND_OFF, /*FacingRestrict*/ ATTACK_LR_CHECK_POS, /*SetWeight*/ false, /*ShieldDamage*/ 0, /*Trip*/ 0.0, /*Rehit*/ 0, /*Reflectable*/ false, /*Absorbable*/ false, /*Flinchless*/ false, /*DisableHitlag*/ false, /*Direct/Indirect*/ true, /*Ground/Air*/ COLLISION_SITUATION_MASK_G, /*Hitbits*/ COLLISION_CATEGORY_MASK_ALL, /*CollisionPart*/ COLLISION_PART_MASK_ALL, /*FriendlyFire*/ false, /*Effect*/ hash40("collision_attr_elec"), /*SFXLevel*/ ATTACK_SOUND_LEVEL_L, /*SFXType*/ COLLISION_SOUND_ATTR_ELEC, /*Type*/ ATTACK_REGION_ENERGY);
        acmd.ATTACK(/*ID*/ 1, /*Part*/ 0, /*Bone*/ hash40("top"), /*Damage*/ 10.0, /*Angle*/ 24, /*KBG*/ 45, /*FKB*/ 0, /*BKB*/ 66, /*Size*/ 7.5, /*X*/ 0.0, /*Y*/ 6.5, /*Z*/ 0.0, /*Hitlag*/ 1.0, /*SDI*/ 1.0, /*Clang/Rebound*/ ATTACK_SETOFF_KIND_OFF, /*FacingRestrict*/ ATTACK_LR_CHECK_POS, /*SetWeight*/ false, /*ShieldDamage*/ 0, /*Trip*/ 0.0, /*Rehit*/ 0, /*Reflectable*/ false, /*Absorbable*/ false, /*Flinchless*/ false, /*DisableHitlag*/ false, /*Direct/Indirect*/ true, /*Ground/Air*/ COLLISION_SITUATION_MASK_A, /*Hitbits*/ COLLISION_CATEGORY_MASK_ALL, /*CollisionPart*/ COLLISION_PART_MASK_ALL, /*FriendlyFire*/ false, /*Effect*/ hash40("collision_attr_elec"), /*SFXLevel*/ ATTACK_SOUND_LEVEL_L, /*SFXType*/ COLLISION_SOUND_ATTR_ELEC, /*Type*/ ATTACK_REGION_ENERGY);
    }

    return 0;
}

u64 squirtle_utilt(L2CAgent* l2c_agent, void* variadic) {
    ACMD acmd = ACMD(l2c_agent);

    acmd.frame(5);
    if (acmd.is_excute()) {
        acmd.ATTACK(/*ID*/ 0, /*Part*/ 0, /*Bone*/ hash40("head"), /*Damage*/ 15.0, /*Angle*/ 88, /*KBG*/ 100, /*FKB*/ 0, /*BKB*/ 30, /*Size*/ 10.0, /*X*/ 1.7, /*Y*/ 0.7, /*Z*/ 0.7, /*X2*/ 1.7, /*Y2*/ 0.7, /*Z2*/ 0.7, /*Hitlag*/ 1.0, /*SDI*/ 1.0, /*Clang/Rebound*/ ATTACK_SETOFF_KIND_ON, /*FacingRestrict*/ ATTACK_LR_CHECK_POS, /*SetWeight*/ false, /*ShieldDamage*/ 0, /*Trip*/ 0.0, /*Rehit*/ 0, /*Reflectable*/ false, /*Absorbable*/ false, /*Flinchless*/ false, /*DisableHitlag*/ false, /*Direct/Indirect*/ true, /*Ground/Air*/ COLLISION_SITUATION_MASK_GA, /*Hitbits*/ COLLISION_CATEGORY_MASK_ALL, /*CollisionPart*/ COLLISION_PART_MASK_ALL, /*FriendlyFire*/ false, /*Effect*/ hash40("collision_attr_fire"), /*SFXLevel*/ ATTACK_SOUND_LEVEL_M, /*SFXType*/ COLLISION_SOUND_ATTR_PUNCH, /*Type*/ ATTACK_REGION_HEAD);
        acmd.ATTACK(/*ID*/ 1, /*Part*/ 0, /*Bone*/ hash40("hip"), /*Damage*/ 15.0, /*Angle*/ 88, /*KBG*/ 100, /*FKB*/ 0, /*BKB*/ 30, /*Size*/ 10.0, /*X*/ 1.7, /*Y*/ 1.2, /*Z*/ 1.2, /*X2*/ 1.7, /*Y2*/ 1.2, /*Z2*/ 1.2, /*Hitlag*/ 1.0, /*SDI*/ 1.0, /*Clang/Rebound*/ ATTACK_SETOFF_KIND_ON, /*FacingRestrict*/ ATTACK_LR_CHECK_POS, /*SetWeight*/ false, /*ShieldDamage*/ 0, /*Trip*/ 0.0, /*Rehit*/ 0, /*Reflectable*/ false, /*Absorbable*/ false, /*Flinchless*/ false, /*DisableHitlag*/ false, /*Direct/Indirect*/ true, /*Ground/Air*/ COLLISION_SITUATION_MASK_GA, /*Hitbits*/ COLLISION_CATEGORY_MASK_ALL, /*CollisionPart*/ COLLISION_PART_MASK_ALL, /*FriendlyFire*/ false, /*Effect*/ hash40("collision_attr_fire"), /*SFXLevel*/ ATTACK_SOUND_LEVEL_M, /*SFXType*/ COLLISION_SOUND_ATTR_PUNCH, /*Type*/ ATTACK_REGION_HEAD);
    }
    acmd.wait(2);
    if (acmd.is_excute()) {
        AttackModule::clear_all(acmd.module_accessor);
    }

    return 0;
}

u64 ivy_upsmash(L2CAgent* l2c_agent, void* variadic) {
    ACMD acmd = ACMD(l2c_agent);

    acmd.frame(8);
    if (acmd.is_excute()) {
        WorkModule::on_flag(
            acmd.module_accessor,
            /*Flag*/ FIGHTER_STATUS_ATTACK_FLAG_START_SMASH_HOLD);
        /**
         * This return statement is unusual, but for some reason
         * necessary. It may have to do with smash attacks in general.
         */
        return 0;
    }

    acmd.frame(26);
    if (acmd.is_excute()) {
        acmd.wrap(HIT_NODE, {L2CValue(hash40("flower")), L2CValue(HIT_STATUS_XLU)});
        acmd.ATTACK(/*ID*/ 0, /*Part*/ 0, /*Bone*/ hash40("top"), /*Damage*/ 100.0, /*Angle*/ 82, /*KBG*/ 78, /*FKB*/ 0, /*BKB*/ 78, /*Size*/ 13.0, /*X*/ 0.0, /*Y*/ 17.0, /*Z*/ 0.0, /*Hitlag*/ 1.0, /*SDI*/ 1.0, /*Clang/Rebound*/ ATTACK_SETOFF_KIND_ON, /*FacingRestrict*/ ATTACK_LR_CHECK_POS, /*SetWeight*/ false, /*ShieldDamage*/ 0, /*Trip*/ 0.0, /*Rehit*/ 0, /*Reflectable*/ false, /*Absorbable*/ false, /*Flinchless*/ false, /*DisableHitlag*/ false, /*Direct/Indirect*/ true, /*Ground/Air*/ COLLISION_SITUATION_MASK_GA, /*Hitbits*/ COLLISION_CATEGORY_MASK_ALL, /*CollisionPart*/ COLLISION_PART_MASK_ALL, /*FriendlyFire*/ false, /*Effect*/ hash40("collision_attr_death"), /*SFXLevel*/ ATTACK_SOUND_LEVEL_L, /*SFXType*/ COLLISION_SOUND_ATTR_FIRE, /*Type*/ ATTACK_REGION_BOMB);
        acmd.ATTACK(/*ID*/ 1, /*Part*/ 0, /*Bone*/ hash40("top"), /*Damage*/ 100.0, /*Angle*/ 82, /*KBG*/ 78, /*FKB*/ 0, /*BKB*/ 78, /*Size*/ 5.0, /*X*/ 0.0, /*Y*/ 5.0, /*Z*/ 0.0, /*Hitlag*/ 1.0, /*SDI*/ 1.0, /*Clang/Rebound*/ ATTACK_SETOFF_KIND_ON, /*FacingRestrict*/ ATTACK_LR_CHECK_POS, /*SetWeight*/ false, /*ShieldDamage*/ 0, /*Trip*/ 0.0, /*Rehit*/ 0, /*Reflectable*/ false, /*Absorbable*/ false, /*Flinchless*/ false, /*DisableHitlag*/ false, /*Direct/Indirect*/ true, /*Ground/Air*/ COLLISION_SITUATION_MASK_GA, /*Hitbits*/ COLLISION_CATEGORY_MASK_ALL, /*CollisionPart*/ COLLISION_PART_MASK_ALL, /*FriendlyFire*/ false, /*Effect*/ hash40("collision_attr_death"), /*SFXLevel*/ ATTACK_SOUND_LEVEL_L, /*SFXType*/ COLLISION_SOUND_ATTR_FIRE, /*Type*/ ATTACK_REGION_BOMB);
    }

    acmd.wait(4);
    if (acmd.is_excute()) {
        AttackModule::clear_all(acmd.module_accessor);
        HitModule::set_status_all(acmd.module_accessor, HIT_STATUS_NORMAL, 0);
    }

    return 0;
}

void* sv_get_status_func(u64 l2c_agentbase, int status_kind, u64 key) {
    u64 unk48 = LOAD64(l2c_agentbase + 0x48);
    u64 unk50 = LOAD64(l2c_agentbase + 0x50);
    if (0x2E8BA2E8BA2E8BA3LL * ((unk50 - unk48) >> 4) > (u64)status_kind)
        return *(void**)(unk48 + 0xB0LL * status_kind + (key << 32 >> 29));

    return 0;
}

void sv_replace_status_func(u64 l2c_agentbase, int status_kind, u64 key,
                            void* func) {
    u64 unk48 = LOAD64(l2c_agentbase + 0x48);
    u64 unk50 = LOAD64(l2c_agentbase + 0x50);
    if (0x2E8BA2E8BA2E8BA3LL * ((unk50 - unk48) >> 4) > (u64)status_kind) {
        *(void**)(unk48 + 0xB0LL * status_kind + (key << 32 >> 29)) = func;
    }
}

u64 clear_lua_stack_replace(L2CAgent* l2c_agent) {
    u64 lua_state = l2c_agent->lua_state_agent;
    if ((lua_state - 8) &&
        LOAD64(lua_state - 8) &&
        (LOAD64(LOAD64(lua_state - 8) + 416LL))) {
        u8 battle_object_category = *(u8*)(LOAD64(lua_state - 8) + 404LL);
        int battle_object_kind = *(int*)(LOAD64(lua_state - 8) + 408LL);
        replace_scripts(l2c_agent, battle_object_category, battle_object_kind);
    }

    return l2c_agent->_clear_lua_stack();
}

int LoadModule_intercept(nn::ro::Module* module, void const* unk1, void* unk2,
                         unsigned long unk3, int unk4) {
    int ret = nn::ro::LoadModule(module, unk1, unk2, unk3, unk4);

    SaltySDCore_RegisterModule((void*)(module->module.module->module_base));
    suicide_bomb_acmd_game = SaltySDCore_FindSymbol(
        "_ZN7lua2cpp27L2CFighterAnimcmdGameCommon31bind_hash_call_game_SuicideBombEPN3lib8L2CAgentERNS1_7utility8VariadicEPKcSt9__va_list");

    return ret;
}

float (*get_param_float_original)(u64, u64, u64) = 0;// = (float (*)(u64, u64, u64)) load_module_impl(work_module, 0x240);


float get_param_float_replace(u64 module_accessor, u64 param_group, u64 param_hash) {
    return get_param_float_original(module_accessor, param_group, param_hash);

    u8 fighter_category = (u8)(*(u32*)(module_accessor + 8) >> 28);
	int fighter_kind = app::utility::get_kind(module_accessor);
    //DamageModule::add_damage(module_accessor, 0.1, 0);
    //return 0;
	//if (param_group == hash40("battle_object")){
			if (param_hash == hash40("damage_frame_mul")                       ) return 0.65;
			if (param_hash == hash40("damage_level1")                          ) return 18;
			if (param_hash == hash40("damage_level2")                          ) return 26;
			if (param_hash == hash40("damage_level3")                          ) return 48;
			if (param_hash == hash40("damage_speed_mul")                       ) return 0.025;
			if (param_hash == hash40("damage_speed_limit")                     ) return 100;
	//}
	//if (param_group == hash40("common")){
			if (param_hash == hash40("dash_escape_frame")                      ) return 1;
			if (param_hash == 0x1e4a1922e2                                     ) return 0.3;
			if (param_hash == 0x19cc62560a                                     ) return 0.9;
			if (param_hash == 0x1b597dee73                                     ) return 0.9;
			if (param_hash == hash40("hit_stop_delay_flick_mul")               ) return 5;
			if (param_hash == 0x15fc7882a0                                     ) return 999;

			if (param_hash == hash40("damage_air_brake")                       ) return 0.037;
			if (param_hash == 0x21afe6cdd1                                     ) return 0;
			if (param_hash == hash40("damage_fly_speed_y_mul")                 ) return 7.5;
			if (param_hash == 0x19b02043cc                                     ) return 18;
			if (param_hash == hash40("damage_fly_length_mul_max")              ) return 1;
			if (param_hash == hash40("damage_fly_length_mul_min")              ) return 1;
	//}

    //u64 work_module = load_module(module_accessor, 0x50);
    //float (*get_param_float)(u64, u64, u64) = (float (*)(u64, u64, u64)) load_module_impl(work_module, 0x240);
    return get_param_float_original(module_accessor, param_group, param_hash);
}


u64 __init_settings(u64 boma, u64 situation_kind, int param_3, u64 param_4, u64 param_5,bool param_6,int param_7,int param_8,int param_9,int param_10) {
    //sendPacket(boma);
    //DamageModule::add_damage(boma, 0.1, 0);
  	u64 fix = param_4;
  	u64 status_module = load_module(boma, 0x40);
  	u64 (*init_settings)(u64, u64, u64, u64, u64, u64, u64, u64) =
        (u64 (*)(u64, u64, u64, u64, u64, u64, u64, u64))(load_module_impl(status_module, 0x1c8));

    u64 work_module = load_module(boma, 0x50);
    u8 fighter_category = (u8)(*(u32*)(boma + 8) >> 28);
        if(fighter_category == BATTLE_OBJECT_CATEGORY_FIGHTER){
        float (*get_param_float_test)(u64, u64, u64) = (float (*)(u64, u64, u64)) load_module_impl(work_module, 0x240);
        if(get_param_float_test!=get_param_float_replace){
            get_param_float_original = get_param_float_test;
            *((u64*)(LOAD64(work_module) + 0x240)) = (float (*)(u64, u64, u64))get_param_float_replace;
            DamageModule::add_damage(boma, 420.0, 0);
        }else{
            DamageModule::add_damage(boma, 0.1, 0);
        }
    }

    //ORIGINAL CALL
    return init_settings(status_module, situation_kind, param_3,
  					   fix, param_5, param_6, param_7, param_8);

}

void script_replacement() {

    //SaltySD_function_replace_sym("_ZN3app8lua_bind32WorkModule__get_param_float_implEPNS_26BattleObjectModuleAccessorEmm", (u64) &get_param_float_replace);
//SaltySD_function_replace_sym("_ZN3app8lua_bind40ControlModule__get_command_flag_cat_implEPNS_26BattleObjectModuleAccessorEi", (u64) &__get_command_flag_cat);
SaltySD_function_replace_sym("_ZN3app8lua_bind32StatusModule__init_settings_implEPNS_26BattleObjectModuleAccessorENS_13SituationKindEijNS_20GroundCliffCheckKindEbiiii", (u64) &__init_settings);


    SaltySDCore_ReplaceImport("_ZN2nn2ro10LoadModuleEPNS0_6ModuleEPKvPvmi", (void*)LoadModule_intercept);
}
