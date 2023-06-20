// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameDataTables.h"
#include "Gladiator.generated.h"

UCLASS(config = Game)
class BELLZ_API AGladiator : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AGladiator();

	// Camera boom positioning the camera behind the character
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	// Follow camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	// 精灵组件用于生成特效，它离摄像机很近
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Effect, meta = (AllowPrivateAccess = "true"))
	class UPaperSpriteComponent* EffectSprite;

	// 照相机旋转速率。Base turn rate, in deg/sec.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseTurnRate;

	// Base look up/down 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseLookUpRate;

	// 跳跃速率
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Attributes")
	float jumppingVelocity;

	// 生死状态
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Attributes")
	bool IsStillAlive;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Attributes")
	bool IsAttacking;

	// 当前激活的武器index（多种武器切换）
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Attributes")
	int32 WeaponIndex;

	// 有时候玩家无法控制
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Attributes")
	bool IsControlable;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game DataTables")
	AGameDataTables* TablesInstance;

	// getter
	UFUNCTION(BlueprintCallable, Category = "PlayerAttributes")
	bool GetIsStillAlive() const { return IsStillAlive; }

	// setter
	UFUNCTION(BlueprintCallable, Category = "PlayerAttributes")
	void OnSetPlayerController(bool status);

	UFUNCTION(BlueprintCallable, Category = "PlayerAttributes")
	void OnChangeHealthByAmount(float usedAmount);


	UFUNCTION(BlueprintCallable, Category = "PlayerAttributes")
	float OnGetHealthAmount() const { return TotalHealth; }

	// UFUNCTION(BlueprintCallable, Category = "PlayerAttributes")
	// void OnPostAttack();


	void Jump();

	void StopJumping();

	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Player Attributes")
	float TotalHealth;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Player Attributes")
	float AttackRange;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "PlayerAttributes")
	void MoveForward(float Value);

	UFUNCTION(BlueprintCallable, Category = "PlayerAttributes")
	void MoveRight(float Value);


	UFUNCTION(BlueprintCallable, Category = "PlayerAttributes")
	void OnAttack();

	UFUNCTION(BlueprintCallable, Category = "PlayerAttributes")
	void OnChangeWeapon();

	//Called via input to turn at a given rate.
	void TurnAtRate(float Rate);
	//Called via input to turn look up/down at a given rate.
	void LookUpAtRate(float Rate);


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
