# 무기 슬롯 순환 교체 설계

## 배경 / 목표

현재 플레이어는 무기를 하나만 장착할 수 있다 (`AEldenCharacter::WeaponClass` 단일 프로퍼티, BeginPlay 때 1회 스폰). 대검(Greatsword)을 두 번째 무기로 추가하기 위해, 우선 무기 이동/대기 애니메이션 없이도 **보유 무기 2개(한손검/대검)를 키 입력으로 순환 교체**할 수 있는 구조를 먼저 만든다.

최종적으로는 엘든링처럼 인벤토리 UI에서 장비칸에 무기를 넣고 교체하는 방향을 지향하지만, 그 UI 작업은 이번 스코프에 포함하지 않는다.

## 스코프

**포함**
- 무기 슬롯(보유 무기 목록) 데이터 구조
- 무기별 콤보 몽타주 데이터를 무기 액터로 이전
- 오른쪽 방향키(신규 Enhanced Input 액션) 입력 시 보유 무기 순환 교체
- 무기 교체 시 액터 보이기/숨기기 + `EquippedWeapon` 갱신
- `CombatComponent`가 현재 장착 무기의 콤보 몽타주를 참조하도록 변경

**제외 (후속 과제)**
- 대검 전용 이동/대기 애니메이션, Linked Anim Layer를 통한 로코모션 교체
- 인벤토리 UI에서 드래그/장착
- 무기별 스탯 밸런싱 (BaseDamage는 이미 무기별로 존재하므로 그대로 사용)

## 아키텍처

### 1. 무기별 애니메이션 데이터 이전 (`AEldenWeapon`)

- `TArray<UAnimMontage*> ComboMontages`를 `UEldenCombatComponent`에서 `AEldenWeapon`으로 이전한다.
  - `AEldenEnemy`는 `UEldenCombatComponent`를 사용하지 않으므로 (플레이어 전용 컴포넌트) 이전해도 적 AI 쪽에 영향 없음.
- `AEldenWeapon`에 `GetComboMontages()` 게터를 추가한다. 기존 `GetIcon()` / `GetSkillName()`과 동일하게 "무기 자신의 데이터는 무기가 들고 있는다" 패턴을 따른다.

### 2. 무기 슬롯 관리 (`AEldenCharacter`)

- `TSubclassOf<AEldenWeapon> WeaponClass` 단일 프로퍼티를 `TArray<TSubclassOf<AEldenWeapon>> WeaponSlots`로 교체한다. 에디터에서 순서대로 등록 (예: [한손검, 대검]).
- `TArray<AEldenWeapon*> SpawnedWeapons`와 `int32 CurrentWeaponIndex`를 추가한다.
- BeginPlay 무기 스폰 로직(`EldenCharacter.cpp:100` 부근)을 `WeaponSlots` 순회로 변경:
  - 슬롯마다 스폰 후 `RightHandSocket`에 부착
  - `CurrentWeaponIndex`(기본 0)에 해당하는 무기만 보이게, 나머지는 `SetActorHiddenInGame(true)`
  - `EquippedWeapon`은 계속 "현재 활성 무기 포인터" 역할 유지 (기존 코드 호환)

### 3. 교체 로직

- 신규 Enhanced Input 액션 `SwitchWeaponAction` 추가 (바인딩은 오른쪽 방향키, 프로젝트 세팅에서 지정).
- `AEldenCharacter::SwitchWeapon()`:
  - 기존 상태머신 게이팅 패턴 준수: `if (GetState() != ECharacterState::Idle) return;`
  - `CurrentWeaponIndex`를 순환 증가 (`(CurrentWeaponIndex + 1) % SpawnedWeapons.Num()`)
  - 이전 무기 Hide, 새 무기 Show
  - `EquippedWeapon = SpawnedWeapons[CurrentWeaponIndex]`로 갱신

### 4. CombatComponent 참조 방식

- `UEldenCombatComponent::ExecuteAttack()`은 자체 멤버 배열 대신 `PlayerCharacter->GetEquippedWeapon()->GetComboMontages()`를 매번 조회한다.
- **캐싱하지 않는 이유**: `GetEquippedWeapon()` / `GetComboMontages()`는 둘 다 `FORCEINLINE` 멤버 접근이라 조회 비용이 거의 없고, 호출도 Tick이 아니라 공격 입력 시점 1회뿐이다. 반대로 이 값을 컴포넌트에 캐싱하면 무기 교체 시점에 캐시를 갱신할 책임이 생기고, 갱신을 놓치면 "대검으로 바꿨는데 한손검 몽타주가 나가는" 동기화 버그나 댕글링 포인터 위험이 생긴다. 조회 비용이 거의 0이고 원본이 자주 바뀌는 값이므로 캐싱하지 않는 쪽이 더 안전하다.

## 건드리지 않는 부분

- `EldenAnimInstance`의 로코모션 State Machine — 대검 이동 애니메이션이 준비되면 별도 작업(Linked Anim Layer 등)으로 진행.
- 인벤토리(`UEldenInventoryComponent`) — 이번엔 무기 슬롯은 캐릭터 쪽에 독립적으로 둔다. 인벤토리 통합은 후속 과제.
- 방패(`EquippedShield`) 관련 로직 — 무기만 대상으로 하며 방패는 그대로.

## 테스트 / 검증 방법

- PIE에서 오른쪽 방향키 입력 시 한손검 ↔ 대검이 손 소켓에서 교체되어 보이는지 확인.
- 각 무기 장착 상태에서 공격 입력 시 해당 무기의 콤보 몽타주가 재생되는지 확인 (한손검 콤보 vs 대검 콤보 구분).
- 공격/구르기/가드 등 Idle이 아닌 상태에서 교체 키를 눌러도 무시되는지 확인 (상태머신 게이팅 동작 확인).
- HUD의 무기 아이콘/스킬명 표시가 교체 후에도 올바른 무기 정보를 반영하는지 확인 (`EquippedWeapon` 갱신 경로 재사용).
