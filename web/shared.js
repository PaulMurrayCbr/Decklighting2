
class SelectedPage {
	static PageLevel = {
		HOMEPAGE: Symbol('HOMEPAGE'),
		SECTION: Symbol('SECTION')
	}

	pageLevel = SelectedPage.PageLevel.HOMEPAGE; // default value

	section = undefined;

	isHome() {
		return this.pageLevel === SelectedPage.PageLevel.HOMEPAGE;
	}

	// call with no args to mean any section
	isSection(section) {
		return this.pageLevel === SelectedPage.PageLevel.SECTION && (section === undefined || this.section === section);
	}

	getSection() {
		if (!this.isSection()) throw new Error(`state exception: no section selected ${this}`);
		return this.section;
	}

	toString() {
		return `SelectedPage { pageLevel: ${this.pageLevel.toString()}, section: ${this.section} }`;
	}

	constructor({ pageLevel = SelectedPage.PageLevel.HOMEPAGE, section } = {}) {
		this.pageLevel = pageLevel;
		this.section = section;
	}

	static ofHome() {
		return new SelectedPage({ pageLevel: SelectedPage.PageLevel.HOMEPAGE });
	}

	static ofSection(section) {
		if (!section) throw new Error('section is null');
		return new SelectedPage({ pageLevel: SelectedPage.PageLevel.SECTION, section });
	}
}
