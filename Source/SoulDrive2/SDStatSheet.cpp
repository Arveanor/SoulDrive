// Fill out your copyright notice in the Description page of Project Settings.

#include "SoulDrive2.h"
#include "SDStatSheet.h"

SDStatSheet::SDStatSheet()
{
	Strength = 0;
	Intelligence = 0;
	Dexterity = 0;
	Spirit = 0;

	Armor = 0;
	Health = 0;
	Dodge = 0;
	ArcaneResist = 0;
	TechResist = 0;
	NatureResist = 0;

	MovementSpeed = 0;
	HealthPerSecond = 0;
	Mana = 0;
	ManaPerSecond = 0;

	MainWeaponAttackSpeed = 0;
	AltWeaponAttackSpeed = 0;
	MainWeaponRange = 0;
	AltWeaponRange = 0;
	MainWeaponCritChance = 0;
	AltWeaponCritChance = 0;
	MainWeaponCritMod = 0;
	AltWeaponCritMod = 0;
	MainWeaponAmmo = 0;
	AltWeaponAmmo = 0;
	MainWeaponReloadSpeed = 0;
	AltWeaponReloadSpeed = 0;
}

SDStatSheet::~SDStatSheet()
{
}

SDStatSheet SDStatSheet::operator+(const SDStatSheet &addedStats)
{
	Strength += addedStats.Strength;
	Intelligence += addedStats.Intelligence;
	Dexterity += addedStats.Dexterity;
	Spirit += addedStats.Spirit;

	Armor += addedStats.Armor;
	Health += addedStats.Health;
	Dodge += addedStats.Dodge;
	ArcaneResist += addedStats.ArcaneResist;
	TechResist += addedStats.TechResist;
	NatureResist += addedStats.NatureResist;

	MovementSpeed += addedStats.MovementSpeed;
	HealthPerSecond += addedStats.HealthPerSecond;
	Mana += addedStats.Mana;
	ManaPerSecond += addedStats.ManaPerSecond;

	MainWeaponAttackSpeed += addedStats.MainWeaponAttackSpeed;
	AltWeaponAttackSpeed += addedStats.AltWeaponAttackSpeed;
	MainWeaponRange += addedStats.MainWeaponRange;
	AltWeaponRange += addedStats.AltWeaponRange;
	MainWeaponCritChance += addedStats.MainWeaponCritChance;
	AltWeaponCritChance += addedStats.AltWeaponCritChance;
	MainWeaponCritMod += addedStats.MainWeaponCritMod;
	AltWeaponCritMod += addedStats.AltWeaponCritMod;
	MainWeaponAmmo += addedStats.MainWeaponAmmo;
	AltWeaponAmmo += addedStats.AltWeaponAmmo;
	MainWeaponReloadSpeed += addedStats.MainWeaponReloadSpeed;
	AltWeaponReloadSpeed += addedStats.AltWeaponReloadSpeed;

	return *this;
}

SDStatSheet SDStatSheet::operator-(const SDStatSheet& lostStats)
{
	Strength -= lostStats.Strength;
	Intelligence -= lostStats.Intelligence;
	Dexterity -= lostStats.Dexterity;
	Spirit -= lostStats.Spirit;

	Armor -= lostStats.Armor;
	Health -= lostStats.Health;
	Dodge -= lostStats.Dodge;
	ArcaneResist -= lostStats.ArcaneResist;
	TechResist -= lostStats.TechResist;
	NatureResist -= lostStats.NatureResist;

	MovementSpeed -= lostStats.MovementSpeed;
	HealthPerSecond -= lostStats.HealthPerSecond;
	Mana -= lostStats.Mana;
	ManaPerSecond -= lostStats.ManaPerSecond;

	MainWeaponAttackSpeed -= lostStats.MainWeaponAttackSpeed;
	AltWeaponAttackSpeed -= lostStats.AltWeaponAttackSpeed;
	MainWeaponRange -= lostStats.MainWeaponRange;
	AltWeaponRange -= lostStats.AltWeaponRange;
	MainWeaponCritChance -= lostStats.MainWeaponCritChance;
	AltWeaponCritChance -= lostStats.AltWeaponCritChance;
	MainWeaponCritMod -= lostStats.MainWeaponCritMod;
	AltWeaponCritMod -= lostStats.AltWeaponCritMod;
	MainWeaponAmmo -= lostStats.MainWeaponAmmo;
	AltWeaponAmmo -= lostStats.AltWeaponAmmo;
	MainWeaponReloadSpeed -= lostStats.MainWeaponReloadSpeed;
	AltWeaponReloadSpeed -= lostStats.AltWeaponReloadSpeed;

	return *this;
}