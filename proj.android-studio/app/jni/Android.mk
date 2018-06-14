LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

$(call import-add-path,$(LOCAL_PATH)/../../../cocos2d)
$(call import-add-path,$(LOCAL_PATH)/../../../cocos2d/external)
$(call import-add-path,$(LOCAL_PATH)/../../../cocos2d/cocos)
$(call import-add-path,$(LOCAL_PATH)/../../../cocos2d/cocos/audio/include)

LOCAL_MODULE := MyGame_shared

LOCAL_MODULE_FILENAME := libMyGame

LOCAL_SRC_FILES := hellocpp/main.cpp \
                   ../../../Classes/AppDelegate.cpp \
                   ../../../Classes/Background.cpp \
                   ../../../Classes/Block.cpp \
                   ../../../Classes/CameraHelper.cpp \
                   ../../../Classes/CastIceBladeStrike.cpp \
                   ../../../Classes/CastLavaTotem.cpp \
                   ../../../Classes/CastLightningStrike.cpp \
                   ../../../Classes/CastMeteorRain.cpp \
                   ../../../Classes/CastThunderBody.cpp \
                   ../../../Classes/Character.cpp \
                   ../../../Classes/CharacterSelectionScene.cpp \
                   ../../../Classes/Charge.cpp \
                   ../../../Classes/ClawAttack.cpp \
                   ../../../Classes/DarkPortal.cpp \
                   ../../../Classes/DeathZone.cpp \
                   ../../../Classes/DragonNova.cpp \
                   ../../../Classes/Dummy.cpp \
                   ../../../Classes/Effect.cpp \
                   ../../../Classes/ElectOrb.cpp \
                   ../../../Classes/Enemy.cpp \
                   ../../../Classes/EnemyApeDino.cpp \
                   ../../../Classes/EnemyFeeder.cpp \
                   ../../../Classes/EnemyRamus.cpp \
                   ../../../Classes/EnemyWerewolf.cpp \
                   ../../../Classes/EnergyBar.cpp \
                   ../../../Classes/EnergyOrb.cpp \
                   ../../../Classes/Fire.cpp \
                   ../../../Classes/FireBall.cpp \
                   ../../../Classes/GameObject.cpp \
                   ../../../Classes/Gold.cpp \
                   ../../../Classes/Ground.cpp \
                   ../../../Classes/GroundSprite.cpp \
                   ../../../Classes/HiddenBlock.cpp \
                   ../../../Classes/HornAttack.cpp \
                   ../../../Classes/HUDLayer.cpp \
                   ../../../Classes/IceBlade.cpp \
                   ../../../Classes/IceBladeStrike.cpp \
                   ../../../Classes/IceDragon.cpp \
                   ../../../Classes/IceShard.cpp \
                   ../../../Classes/IceWall.cpp \
                   ../../../Classes/IconSkill.cpp \
                   ../../../Classes/IntroScene.cpp \
                   ../../../Classes/Item.cpp \
                   ../../../Classes/JawAttack.cpp \
                   ../../../Classes/LavaTotem.cpp \
                   ../../../Classes/LightningStrike.cpp \
                   ../../../Classes/Meat.cpp \
                   ../../../Classes/MenuScene1.cpp \
                   ../../../Classes/MovingBlock.cpp \
                   ../../../Classes/MyPoolManager.cpp \
                   ../../../Classes/NextStageZone.cpp \
                   ../../../Classes/Player.cpp \
                   ../../../Classes/PlayScene.cpp \
                   ../../../Classes/Projectile.cpp \
                   ../../../Classes/PushableBlock.cpp \
                   ../../../Classes/RedDino.cpp \
                   ../../../Classes/ResourceManager.cpp \
                   ../../../Classes/RollAttack.cpp \
                   ../../../Classes/SelfExplode.cpp \
                   ../../../Classes/Spike.cpp \
                   ../../../Classes/Teleporter.cpp \
                   ../../../Classes/TextEffect.cpp \
                   ../../../Classes/ThunderBody.cpp \
                   ../../../Classes/TriggerBlockZone.cpp \
                   ../../../Classes/WhiteDino.cpp \
                   ../../../Classes/WorldScene.cpp \
                   ../../../Classes/YClawAttack.cpp \
                   ../../../Classes/YellowDino.cpp \
                   ../../../Classes/ZeusWrath.cpp \
                   ../../../Classes/Zone.cpp \
                   ../../../Classes/Speecher.cpp \
                   ../../../Classes/SpeechModel.cpp \
                   ../../../Classes/SpeechZone.cpp \
                   ../../../Classes/EnemyHopper.cpp

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../../Classes

# _COCOS_HEADER_ANDROID_BEGIN
# _COCOS_HEADER_ANDROID_END


LOCAL_STATIC_LIBRARIES := cocos2dx_static

# _COCOS_LIB_ANDROID_BEGIN
# _COCOS_LIB_ANDROID_END

include $(BUILD_SHARED_LIBRARY)

$(call import-module,.)

# _COCOS_LIB_IMPORT_ANDROID_BEGIN
# _COCOS_LIB_IMPORT_ANDROID_END
