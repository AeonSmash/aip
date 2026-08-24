#include "AIPSovereigntyWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/TextBlock.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Framework/Text/TextLayout.h"
#include "Widgets/Layout/Anchors.h"

void UAIPSovereigntyWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (!SummaryText && WidgetTree)
	{
		UCanvasPanel* Root = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("RootCanvas"));
		WidgetTree->RootWidget = Root;
		RuntimeText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("Summary"));
		ArenaText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("Arena"));
		if (Root && RuntimeText)
		{
			UCanvasPanelSlot* PanelSlot = Root->AddChildToCanvas(RuntimeText);
			if (PanelSlot)
			{
				PanelSlot->SetAutoSize(true);
				PanelSlot->SetPosition(FVector2D(32.f, 88.f));
			}
			RuntimeText->SetText(FText::FromString(CachedSummary));
			FSlateFontInfo Font = RuntimeText->GetFont();
			Font.Size = 16;
			RuntimeText->SetFont(Font);
		}
		if (Root && ArenaText)
		{
			UCanvasPanelSlot* ArenaSlot = Root->AddChildToCanvas(ArenaText);
			if (ArenaSlot)
			{
				ArenaSlot->SetAutoSize(true);
				ArenaSlot->SetPosition(FVector2D(32.f, 32.f));
			}
			ArenaText->SetText(FText::FromString(CachedArena));
			FSlateFontInfo Font = ArenaText->GetFont();
			Font.Size = 18;
			ArenaText->SetFont(Font);
		}
		AwardText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("Award"));
		if (Root && AwardText)
		{
			UCanvasPanelSlot* AwardSlot = Root->AddChildToCanvas(AwardText);
			if (AwardSlot)
			{
				AwardSlot->SetAnchors(FAnchors(0.5f, 0.42f, 0.5f, 0.42f));
				AwardSlot->SetAlignment(FVector2D(0.5f, 0.5f));
				AwardSlot->SetAutoSize(true);
				AwardSlot->SetPosition(FVector2D(0.f, 0.f));
			}
			AwardText->SetJustification(ETextJustify::Center);
			AwardText->SetColorAndOpacity(FSlateColor(FLinearColor(0.95f, 0.78f, 0.22f, 1.f)));
			FSlateFontInfo Font = AwardText->GetFont();
			Font.Size = 72;
			AwardText->SetFont(Font);
			AwardText->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
	else if (SummaryText)
	{
		SummaryText->SetText(FText::FromString(CachedSummary));
	}
}

void UAIPSovereigntyWidget::SetArenaStatus(const FString& Status)
{
	CachedArena = Status;
	if (ArenaText)
	{
		ArenaText->SetText(FText::FromString(CachedArena));
	}
}

void UAIPSovereigntyWidget::SetSummary(const FString& Summary)
{
	CachedSummary = Summary;
	if (SummaryText)
	{
		SummaryText->SetText(FText::FromString(Summary));
	}
	if (RuntimeText)
	{
		RuntimeText->SetText(FText::FromString(Summary));
	}
}

void UAIPSovereigntyWidget::ShowAward(const FString& Line)
{
	if (AwardText)
	{
		AwardText->SetText(FText::FromString(Line));
		AwardText->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(AwardTimer);
		World->GetTimerManager().SetTimer(AwardTimer, this, &UAIPSovereigntyWidget::ClearAward, 4.5f, false);
	}
}

void UAIPSovereigntyWidget::ClearAward()
{
	if (AwardText)
	{
		AwardText->SetVisibility(ESlateVisibility::Collapsed);
	}
}
