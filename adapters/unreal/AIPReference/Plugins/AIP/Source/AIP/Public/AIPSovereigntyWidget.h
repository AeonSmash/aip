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

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> SummaryText;

	UPROPERTY()
	TObjectPtr<UTextBlock> RuntimeText;

	FString CachedSummary = TEXT("AIP: awaiting envelope");
};
