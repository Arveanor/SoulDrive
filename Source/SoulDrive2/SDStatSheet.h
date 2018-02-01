// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

/**
 * 
 */
class SOULDRIVE2_API SDStatSheet
{

public:
	SDStatSheet();
	~SDStatSheet();

	uint16 Strength;
	uint16 Intelligence;
	uint16 Dexterity;
	uint16 Spirit;

	uint16 Armor;
	uint16 Health;
	uint16 Dodge;
	uint16 ArcaneResist;
	uint16 TechResist;
	uint16 NatureResist;

	uint16 MovementSpeed;
	uint16 HealthPerSecond;
	uint16 Mana;
	uint16 ManaPerSecond;

	uint16 MainWeaponAttackSpeed;
	uint16 AltWeaponAttackSpeed;
	uint16 MainWeaponRange;
	uint16 AltWeaponRange;
	uint16 MainWeaponCritChance;
	uint16 AltWeaponCritChance;
	uint16 MainWeaponCritMod;
	uint16 AltWeaponCritMod;
	uint16 MainWeaponAmmo;
	uint16 AltWeaponAmmo;
	uint16 MainWeaponReloadSpeed;
	uint16 AltWeaponReloadSpeed;

	SDStatSheet operator+(const SDStatSheet& addedStats);
	SDStatSheet operator-(const SDStatSheet& lostStats);
};
