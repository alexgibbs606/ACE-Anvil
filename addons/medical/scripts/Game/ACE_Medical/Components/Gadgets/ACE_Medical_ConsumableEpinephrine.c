//! Epinephrine effect: Heals resilience hit zone
[BaseContainerProps()]
class ACE_Medical_ConsumableEpinephrine : SCR_ConsumableEffectHealthItems
{
	//------------------------------------------------------------------------------------------------
	//! Can be applied when patient is unconscious and no epinephrine is in the system
	override bool CanApplyEffect(notnull IEntity target, notnull IEntity user, out SCR_EConsumableFailReason failReason)
	{
		ChimeraCharacter char = ChimeraCharacter.Cast(target);
		if (!char)
			return false;

		SCR_CharacterDamageManagerComponent damageManager = SCR_CharacterDamageManagerComponent.Cast(char.GetDamageManager());
		if (!damageManager)
			return false;

		SCR_HitZone resilienceHZ = SCR_HitZone.Cast(damageManager.GetResilienceHitZone());
		if (!resilienceHZ)
			return false;

		// Check if epinephrine is in the system already
		array<ref PersistentDamageEffect> effects = damageManager.GetAllPersistentEffectsOfType(ACE_Medical_EpinephrineDamageEffect);
		if (!effects.IsEmpty())
		{
			failReason = SCR_EConsumableFailReason.ALREADY_APPLIED;
			return false;
		}
		
		// Cannot be applied while bleeding
		if (damageManager.IsBleeding())
		{
			failReason = SCR_EConsumableFailReason.IS_BLEEDING;
			return false;
		}

		// In the event the patient is critically injured, we don't allow epi
		if (damageManager.ACE_Medical_HasCriticalHealth() && damageManager.ACE_Medical_CanBeHealed(atMedicalFacility: true))
		{
			// If the patient can still be healed with the medicalkit, we'll just say that the patient is too injured
			if (damageManager.ACE_Medical_CanBeHealed(atMedicalFacility: false))	
				failReason = SCR_EConsumableFailReason.ACE_MEDICAL_TOO_DAMAGED;
			// Otherwise, we'll display a seperate message showing the patient as critically injured
			else
				failReason = SCR_EConsumableFailReason.ACE_MEDICAL_TOO_DAMAGED_MEDICAL_FACILITY_REQUIRED;

			return false;
		}
		
		SCR_CharacterControllerComponent charCtrl = SCR_CharacterControllerComponent.Cast(char.GetCharacterController());
		if (charCtrl.GetLifeState() == ECharacterLifeState.INCAPACITATED)
			return true;

		failReason = SCR_EConsumableFailReason.ACE_MEDICAL_NOT_INCAPACITATED;
		return false;
	}

	//------------------------------------------------------------------------------------------------
	//! Can be applied to hit zones under same conditions as CanApplyEffect
	override bool CanApplyEffectToHZ(notnull IEntity target, notnull IEntity user, ECharacterHitZoneGroup group, out SCR_EConsumableFailReason failReason = SCR_EConsumableFailReason.NONE)
	{
		return CanApplyEffect(target, user, failReason);
	}

	//------------------------------------------------------------------------------------------------
	//! Set consumable type in ctor
	void ACE_Medical_ConsumableEpinephrine()
	{
		m_eConsumableType = SCR_EConsumableType.ACE_MEDICAL_EPINEPHRINE;
	}
}
