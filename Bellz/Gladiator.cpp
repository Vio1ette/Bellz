// Fill out your copyright notice in the Description page of Project Settings.


#include "Bellz.h"
#include "Gladiator.h"
#include "Components/CapsuleComponent.h"
#include "PaperSpriteComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EngineUtils.h"
// Sets default values
AGladiator::AGladiator()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Set size for collision capsule
	// UCapsuleComponent在Character.h里只是个前置声明，要找这个类的定义，得我们自己包含Components/CapsuleComponent.h
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	// GetCapsuleComponent();
	TotalHealth = 100.f;
	AttackRange = 25.f;

	jumppingVelocity = 600.f;

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// 当Controller旋转的时候不要旋转，只影响相机
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// 配置 Character Movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	// 旋转速度
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
	// Z轴跳跃速度
	GetCharacterMovement()->JumpZVelocity = jumppingVelocity;
	GetCharacterMovement()->AirControl = 0.2f;

	// 相机臂
	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);
	CameraBoom->TargetArmLength = 300.0f;
	CameraBoom->bUsePawnControlRotation = true; 	// Rotate the arm based on the controller，Arm组件要跟着controller转

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->AttachToComponent(CameraBoom, FAttachmentTransformRules::KeepRelativeTransform, USpringArmComponent::SocketName); 	// 把相机Attach到Arm的末端，SocketName貌似代表末端点，
	FollowCamera->bUsePawnControlRotation = false;	// 相机相对Arm应该是静止的

	EffectSprite = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("ClawEffect"));
	EffectSprite->AttachToComponent(CameraBoom, FAttachmentTransformRules::KeepRelativeTransform);

	IsStillAlive = true;
	IsAttacking = false;
	WeaponIndex = 1;

	OnSetPlayerController(true);
	
}

// Called when the game starts or when spawned
void AGladiator::BeginPlay()
{
	Super::BeginPlay();


	for (TActorIterator<AGameDataTables> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		if (ActorItr)
		{
			TablesInstance = *ActorItr;
			TablesInstance->OnFetchAllTables();
		}
	}
}

// Called every frame
void AGladiator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AGladiator::SetupPlayerInputComponent(class UInputComponent* MyInputComponent)
{

	Super::SetupPlayerInputComponent(MyInputComponent);
	// Set up gameplay key bindings
	check(MyInputComponent);
	MyInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	MyInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	MyInputComponent->BindAction("Attack", IE_Released, this, &AGladiator::OnAttack);
	MyInputComponent->BindAction("ChangeWeapon", IE_Released, this, &AGladiator::OnChangeWeapon);

	MyInputComponent->BindAxis("MoveForward", this, &AGladiator::MoveForward);
	MyInputComponent->BindAxis("MoveRight", this, &AGladiator::MoveRight);

	//We have 2 versions of the rotation bindings to handle different kinds of devices differently
	//"turn" handles devices that provide an absolute delta, such as a mouse.
	//"turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	MyInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	MyInputComponent->BindAxis("TurnRate", this, &AGladiator::TurnAtRate);
	MyInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	MyInputComponent->BindAxis("LookUpRate", this, &AGladiator::LookUpAtRate);
}

void AGladiator::Jump()
{
  if (IsControlable && !IsAttacking)
  {
    bPressedJump = true;
    JumpKeyHoldTime = 0.0f;
  }
}

void AGladiator::StopJumping()
{
  if (IsControlable)
  {
    bPressedJump = false;
    JumpKeyHoldTime = 0.0f;
  }
}

void AGladiator::OnAttack()
{
  if (IsControlable)
  {
    // 攻击动画要用这个值
    IsAttacking = true;
  }
}

void AGladiator::OnChangeWeapon()
{
  if (IsControlable)
  {
    
    // if (WeaponIndex < TablesInstance->AllWeaponsData.Num())
    // {
    //   WeaponIndex++;
    // }
    // else
    // {
    //   WeaponIndex = 1;
    // }
  }
}


void AGladiator::TurnAtRate(float Rate)
{
  if (IsControlable)
  {
    //calculate delta for this frame from the rate information
    AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
  }
}

void AGladiator::LookUpAtRate(float Rate)
{
  if (IsControlable)
  {
    //calculate delta for this frame from the rate information
    AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
  }
}

void AGladiator::OnSetPlayerController(bool status)
{
  IsControlable = status;
}

void AGladiator::OnChangeHealthByAmount(float usedAmount)
{
	TotalHealth -= usedAmount;
	FOutputDeviceNull ar;
	this->CallFunctionByNameWithArguments(TEXT("ApplyGetDamageEffect"), ar, NULL, true);
}

void AGladiator::MoveForward(float Value)
{
  if ((Controller != NULL) && (Value != 0.0f) && IsControlable && !IsAttacking)
  {
    //find out which way is forward(Controller的Yaw方向就是前进的方向)
    const FRotator Rotation = Controller->GetControlRotation();
    const FRotator YawRotation(0, Rotation.Yaw, 0);

    //get forward vector
    const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);   // 虚幻的坐标系，X应该是前后
    AddMovementInput(Direction, Value);
  }
}

void AGladiator::MoveRight(float Value)
{
  if ((Controller != NULL) && (Value != 0.0f) && IsControlable && !IsAttacking)
  {
    //find out which way is right
    const FRotator Rotation = Controller->GetControlRotation();
    const FRotator YawRotation(0, Rotation.Yaw, 0);

    //get right vector 
    const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);	// 虚幻的坐标系，Y应该是左右
    //add movement in that direction
    AddMovementInput(Direction, Value);
  }
}
