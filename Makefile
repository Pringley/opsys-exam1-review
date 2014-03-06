OUTPUT = opsys-exam1-review.pdf
SOURCE = main.markdown
# TEMPLATE = template.tex

PANDOC = pandoc
FLAGS = --smart

.PHONY: clean

$(OUTPUT): $(SOURCE)
	$(PANDOC) $(FLAGS) --output $@ -- $<

clean:
	rm -f $(OUTPUT)
