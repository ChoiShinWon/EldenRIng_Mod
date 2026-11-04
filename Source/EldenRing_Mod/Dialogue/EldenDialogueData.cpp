
#include "EldenRing_Mod/Dialogue/EldenDialogueData.h"

FText UEldenDialogueData::GetSpeakerName() const
{
	return NPCName;
}

int32 UEldenDialogueData::GetLineCount() const
{
	return Lines.Num();
}

FText UEldenDialogueData::GetLine(int32 Index) const
{
	if (Lines.IsValidIndex(Index))
	{
		return Lines[Index];
	}
	else
	{
		return FText::GetEmpty();
	}
}

bool UEldenDialogueData::IsLastLine(int32 Index) const
{
	return Index >= GetLineCount() - 1;
}
