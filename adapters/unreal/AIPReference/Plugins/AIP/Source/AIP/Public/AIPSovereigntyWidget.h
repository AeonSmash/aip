#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AIPSovereigntyWidget.generated.h"

class UTextBlock;

/** On-screen destination-sovereignty readout. */
UCLASS()
class AIP_API UAIPSovereigntyWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "AIP")
	void SetSummary(const FString& Summary);

	UFUNCTION(BlueprintCallable, Category = "AIP")
	void SetArenaStatus(const FString& Status);

	UFUNCTION(BlueprintCallable, Category = "AIP")
	void ShowAward(const FString& Line);

protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void ClearAward();

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> SummaryText;

	UPROPERTY()
	TObjectPtr<UTextBlock> RuntimeText;

	UPROPERTY()
	TObjectPtr<UTextBlock> ArenaText;

	UPROPERTY()
	TObjectPtr<UTextBlock> AwardText;

	FTimerHandle AwardTimer;

	FString CachedSummary = TEXT("AIP: awaiting envelope");
	FString CachedArena = TEXT("arena: waiting");
};
