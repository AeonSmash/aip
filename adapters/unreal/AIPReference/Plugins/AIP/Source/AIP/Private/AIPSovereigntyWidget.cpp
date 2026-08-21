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
		if (Root && RuntimeText)
		{
			UCanvasPanelSlot* PanelSlot = Root->AddChildToCanvas(RuntimeText);
			if (PanelSlot)
			{
				PanelSlot->SetAutoSize(true);
				PanelSlot->SetPosition(FVector2D(32.f, 32.f));
			}
			RuntimeText->SetText(FText::FromString(CachedSummary));
			FSlateFontInfo Font = RuntimeText->GetFont();
			Font.Size = 16;
			RuntimeText->SetFont(Font);
		}
	}
	else if (SummaryText)
	{
		SummaryText->SetText(FText::FromString(CachedSummary));
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
