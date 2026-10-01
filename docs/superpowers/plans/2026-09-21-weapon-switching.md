# 무기 슬롯 순환 교체 Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.
>
> **⚠️ 프로젝트 특수 규칙 (CLAUDE.md):** 이 저장소는 사용자가 언리얼 C++를 직접 배우는 용도라, Claude는 완성된 복붙 코드를 주지 않는다. 아래 각 Step의 코드 블록은 **힌트 골격(선언부 + TODO 주석)**이며, 실제 함수 본문 구현은 사용자가 직접 채운다. 실행자는 스텝을 "완성 코드 붙여넣기"가 아니라 "사용자에게 골격을 제시하고 채워오게 한 뒤 리뷰"로 수행해야 한다. 자동화 테스트 프레임워크가 없으므로 각 태스크의 "테스트" 단계는 PIE(Play In Editor) 수동 확인 체크리스트로 대체한다.

**Goal:** 플레이어가 보유한 무기(한손검/대검) 2개를 키 입력 하나로 순환 교체할 수 있게 한다.

**Architecture:** 콤보 몽타주 데이터를 `UEldenCombatComponent`에서 `AEldenWeapon`으로 이전하고, `AEldenCharacter`는 무기 클래스 배열(`WeaponSlots`)을 BeginPlay에 전부 스폰해 숨겨두었다가, 새 Enhanced Input 액션 입력 시 보이기/숨기기와 `EquippedWeapon` 포인터 전환만으로 무기를 바꾼다.

**Tech Stack:** UE 5.5, C++, Enhanced Input

**Spec:** [docs/superpowers/specs/2026-09-21-weapon-switching-design.md](../specs/2026-09-21-weapon-switching-design.md)

## Global Constraints

- 완성 코드 금지 — 모든 구현 스텝은 힌트 골격만 제공, 본문은 사용자가 작성 (CLAUDE.md)
- `AEldenEnemy`의 전투 로직에는 영향 없음 (별도 컴포넌트 미사용 확인됨)
- 기존 상태머신 패턴(`GetState() != ECharacterState::Idle`이면 return) 준수
- 응답/주석 언어는 한국어, UTF-8 저장

---

### Task 1: 콤보 몽타주 데이터를 `AEldenWeapon`으로 이전

**Files:**
- Modify: `Source/EldenRIng_Mod/Weapon/EldenWeapon.h`
- Modify: `Source/EldenRIng_Mod/Weapon/EldenWeapon.cpp`
- Modify: `Source/EldenRIng_Mod/Component/EldenCombatComponent.h`
- Modify: `Source/EldenRIng_Mod/Component/EldenCombatComponent.cpp`

**Interfaces:**
- Produces: `AEldenWeapon::GetComboMontages() const` → `const TArray<UAnimMontage*>&` 반환. 이후 Task 2, 4에서 이 게터를 사용.

- [ ] **Step 1: `AEldenWeapon.h`에 콤보 몽타주 배열 + 게터 골격 추가**

`EldenWeapon.h`의 `protected` 섹션(기존 `BaseDamage` 근처)에 추가:

```cpp
// TODO: EldenCombatComponent.h의 ComboMontages를 여기로 옮겨오기
// - UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Combat")
// - TArray<UAnimMontage*> 타입, 이름은 ComboMontages로 유지 (기존 코드/블루프린트와 이름 맞추면 마이그레이션 쉬움)
```

`public` 섹션(기존 `GetIcon()`/`GetSkillName()` 근처)에 추가:

```cpp
// TODO: ComboMontages를 읽기 전용으로 리턴하는 게터 선언
// - FORCEINLINE const TArray<UAnimMontage*>& GetComboMontages() const { ... }
// - GetIcon()/GetSkillName()과 동일한 "FORCEINLINE 게터" 패턴 따르기
```

- [ ] **Step 2: `EldenCombatComponent.h`에서 `ComboMontages` 멤버 제거**

기존 [EldenCombatComponent.h:27-28](Source/EldenRIng_Mod/Component/EldenCombatComponent.h:27) 부분:

```cpp
// TODO: 아래 두 줄(UPROPERTY + TArray<UAnimMontage*> ComboMontages) 삭제
// 콤보 몽타주는 이제 무기가 들고 있으므로 컴포넌트에 중복으로 안 둠
```

- [ ] **Step 3: `ExecuteAttack()` / `CheckComboQueue()`가 무기에서 몽타주를 조회하도록 수정**

`EldenCombatComponent.cpp`에서 `ComboMontages[...]`를 참조하던 부분마다:

```cpp
// TODO: ComboMontages 직접 참조 대신 아래 순서로 변경
// 1. PlayerCharacter->GetEquippedWeapon()이 nullptr인지 방어 체크 (무기 없이 스폰되는 경우 대비)
// 2. const TArray<UAnimMontage*>& CurrentCombos = PlayerCharacter->GetEquippedWeapon()->GetComboMontages();
// 3. 기존에 ComboMontages[ComboCount] 쓰던 자리를 CurrentCombos[ComboCount]로 교체
```

> **왜 캐싱하지 않는가**: `GetEquippedWeapon()`/`GetComboMontages()`는 `FORCEINLINE` 멤버 접근이라 사실상 공짜고, 호출 시점도 Tick이 아니라 공격 입력 1회뿐이다. 반대로 캐싱하면 무기 교체 시 캐시 무효화 책임이 생겨 "무기 바꿨는데 이전 콤보가 나가는" 동기화 버그 위험이 생긴다.

- [ ] **Step 4: PIE 수동 확인**

에디터에서 기존 무기(한손검) 하나만 장착된 상태로 플레이해서:
- 공격 입력 시 콤보 몽타주가 이전과 동일하게 재생되는지 확인 (회귀 확인)
- 에디터 컴파일 경고/에러 없는지 확인 (Live Coding 또는 재빌드)

- [ ] **Step 5: Commit**

```bash
git add Source/EldenRIng_Mod/Weapon/EldenWeapon.h Source/EldenRIng_Mod/Weapon/EldenWeapon.cpp Source/EldenRIng_Mod/Component/EldenCombatComponent.h Source/EldenRIng_Mod/Component/EldenCombatComponent.cpp
git commit -m "refactor: 콤보 몽타주 데이터를 AEldenWeapon으로 이전"
```

(GitHub Desktop 사용 시: 위 4개 파일 스테이징 후 커밋 메시지 `refactor: 콤보 몽타주 데이터를 AEldenWeapon으로 이전`)

---

### Task 2: `AEldenCharacter` 무기 슬롯 배열 도입

**Files:**
- Modify: `Source/EldenRIng_Mod/Character/EldenCharacter.h`
- Modify: `Source/EldenRIng_Mod/Character/EldenCharacter.cpp`

**Interfaces:**
- Consumes: Task 1의 `AEldenWeapon::GetComboMontages()` (직접 사용 안 하지만 무기 액터 스폰 로직 재사용)
- Produces: `TArray<AEldenWeapon*> SpawnedWeapons`, `int32 CurrentWeaponIndex` — Task 3에서 교체 로직이 사용.

- [ ] **Step 1: `WeaponClass` 단일 프로퍼티를 배열로 교체**

[EldenCharacter.h:140-141](Source/EldenRIng_Mod/Character/EldenCharacter.h:140) 부분:

```cpp
// TODO: 기존 아래 줄
//   TSubclassOf<class AEldenWeapon> WeaponClass;
// 를 배열로 교체
//   UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
//   TArray<TSubclassOf<class AEldenWeapon>> WeaponSlots;
```

같은 섹션에 추가:

```cpp
// TODO: 스폰된 무기 액터들을 보관할 배열 + 현재 인덱스
// - UPROPERTY() TArray<class AEldenWeapon*> SpawnedWeapons;
// - int32 CurrentWeaponIndex = 0;
// (EquippedWeapon 포인터는 그대로 유지 — "현재 활성 무기"로 계속 사용)
```

- [ ] **Step 2: BeginPlay 스폰 로직을 배열 순회로 변경**

[EldenCharacter.cpp:98-120](Source/EldenRIng_Mod/Character/EldenCharacter.cpp:98) 블록:

```cpp
// TODO: if (WeaponClass != nullptr) { ... } 블록을 for (WeaponSlots 순회)로 변경
// 각 반복에서:
// 1. SpawnActor<AEldenWeapon>(슬롯 클래스, ...) — 기존 SpawnParams 재사용
// 2. RightHandSocket에 AttachToComponent (기존 AttachmentRules 재사용)
// 3. SpawnedWeapons.Add(스폰된 액터)
// 4. CurrentWeaponIndex(기본 0)가 아니면 SetActorHiddenInGame(true)
// 5. 루프 끝나고 EquippedWeapon = SpawnedWeapons.IsValidIndex(0) ? SpawnedWeapons[0] : nullptr;
```

- [ ] **Step 3: PIE 수동 확인**

`WeaponSlots`에 한손검 클래스 하나만 넣고 플레이:
- 기존과 동일하게 손에 무기가 붙어서 보이는지
- HUD 무기 아이콘이 정상 표시되는지 ([EldenCharacter.cpp:160-164](Source/EldenRIng_Mod/Character/EldenCharacter.cpp:160) 경로 재사용 확인)

- [ ] **Step 4: Commit**

```bash
git add Source/EldenRIng_Mod/Character/EldenCharacter.h Source/EldenRIng_Mod/Character/EldenCharacter.cpp
git commit -m "feat: 무기 슬롯 배열 도입 및 BeginPlay 스폰 로직 변경"
```

---

### Task 3: 무기 교체 입력 및 `SwitchWeapon()` 구현

**Files:**
- Modify: `Source/EldenRIng_Mod/Character/EldenCharacter.h`
- Modify: `Source/EldenRIng_Mod/Character/EldenCharacter.cpp`
- Unreal Editor: `IMC_Default`에 신규 `IA_SwitchWeapon` 입력 액션 추가 (에셋 작업, C++ 아님)

**Interfaces:**
- Consumes: Task 2의 `SpawnedWeapons`, `CurrentWeaponIndex`, `EquippedWeapon`

- [ ] **Step 1: 에디터에서 `IA_SwitchWeapon` Input Action 에셋 생성 + `IMC_Default`에 오른쪽 방향키 바인딩**

(코드 아님 — 언리얼 에디터 Content Browser에서 기존 `IA_SwitchItem` 만든 방식 그대로 따라 하기)

- [ ] **Step 2: 헤더에 입력 액션 프로퍼티 + 함수 선언 추가**

[EldenCharacter.h:95-96](Source/EldenRIng_Mod/Character/EldenCharacter.h:95) 근처(다른 `UInputAction*` 프로퍼티들 옆)에:

```cpp
// TODO: 다른 UInputAction* 프로퍼티와 동일한 패턴
// UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
// UInputAction* SwitchWeaponAction;
```

`SwitchItem()` 선언 근처([EldenCharacter.h:217](Source/EldenRIng_Mod/Character/EldenCharacter.h:217))에:

```cpp
// TODO: void SwitchWeapon(); 선언
```

- [ ] **Step 3: `SetupPlayerInputComponent`에 바인딩 추가**

`EldenCharacter.cpp`에서 기존 `SwitchItemAction` 바인딩 라인을 찾아 그 옆에:

```cpp
// TODO: EnhancedInputComponent->BindAction(SwitchWeaponAction, ETriggerEvent::Started, this, &AEldenCharacter::SwitchWeapon);
// (다른 액션 바인딩과 동일한 트리거 이벤트/패턴 사용)
```

- [ ] **Step 4: `SwitchWeapon()` 본문 골격**

```cpp
// TODO: AEldenCharacter::SwitchWeapon() 구현
// void AEldenCharacter::SwitchWeapon()
// {
//     1. 상태 게이팅: if (GetState() != ECharacterState::Idle) return;
//     2. 보유 무기가 2개 미만이면 return (SpawnedWeapons.Num() < 2)
//     3. 현재 무기 Hide: SpawnedWeapons[CurrentWeaponIndex]->SetActorHiddenInGame(true);
//     4. CurrentWeaponIndex = (CurrentWeaponIndex + 1) % SpawnedWeapons.Num();
//     5. 새 무기 Show: SpawnedWeapons[CurrentWeaponIndex]->SetActorHiddenInGame(false);
//     6. EquippedWeapon = SpawnedWeapons[CurrentWeaponIndex];
// }
```

> 참고: `SetEquippedItemsHidden()`([EldenCharacter.cpp:683](Source/EldenRIng_Mod/Character/EldenCharacter.cpp:683))이 전체를 숨기는 별도 경로이므로, 이 함수와 `SwitchWeapon()`이 동시에 호출될 때(예: 포션 마시는 중 교체 시도) 상태가 꼬이지 않는지 리뷰 때 같이 확인.

- [ ] **Step 5: PIE 수동 확인**

`WeaponSlots`에 한손검+대검 두 개를 넣고 플레이:
- 오른쪽 방향키 입력 시 무기가 한손검 ↔ 대검으로 바뀌어 보이는지
- 공격 입력 시 각 무기에 맞는 콤보 몽타주가 나가는지 (Task 1에서 이전한 데이터 확인)
- 공격/구르기/가드 중 교체 키를 눌러도 무시되는지 (Idle 게이팅 확인)
- HUD 아이콘/스킬명이 교체 후 즉시 갱신되는지

- [ ] **Step 6: Commit**

```bash
git add Source/EldenRIng_Mod/Character/EldenCharacter.h Source/EldenRIng_Mod/Character/EldenCharacter.cpp
git commit -m "feat: 무기 슬롯 순환 교체 입력 및 로직 추가"
```

---

## Self-Review 결과

- **스펙 커버리지**: 데이터 이전(Task 1), 슬롯 관리(Task 2), 교체 로직(Task 3) 모두 스펙의 "아키텍처" 4개 섹션에 대응. 스펙의 "제외" 항목(로코모션 애니메이션, 인벤토리 UI, 무기 스탯 밸런싱)은 계획에 포함하지 않음 — 의도대로.
- **Placeholder 스캔**: 코드 블록은 CLAUDE.md 규칙에 따라 의도적으로 힌트 골격(TODO 주석)이며, 이는 "완성 코드 금지"라는 프로젝트 규칙에 따른 것으로 스킬의 기본 "No Placeholders" 규칙에 대한 명시적 예외임 (본문 상단에 명기).
- **타입/이름 일관성**: `GetComboMontages()`, `SpawnedWeapons`, `CurrentWeaponIndex`, `EquippedWeapon`, `SwitchWeaponAction`, `SwitchWeapon()` 이름이 Task 1~3에서 동일하게 사용됨.
