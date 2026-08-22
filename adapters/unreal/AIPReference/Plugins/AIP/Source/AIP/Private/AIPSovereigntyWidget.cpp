#include "AIPSovereigntyWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/TextBlock.h"

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
