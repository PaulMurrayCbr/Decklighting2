
const { useState, useEffect, useRef } = React;


function SectionPage({ name, loading, pixelState, info, apiSection, home }) {

	const [section, setSection] = useState({});
	const [selected, setSelected] = useState({ main: true, color: 0 });
	const [nColors, setNColors] = useState(0);

	useEffect(() => {
		setSection(pixelState[name] ?? {});
	}, [pixelState, name])

	useEffect(() => {
		const n = info?.effectInfo[section?.effect]?.ncolorranges ?? 0;
		setNColors(n);

		if (!selected.main && selected.color >= n) {
			selectMainPage();
		}
	}, [section, info])


	function reload() {
		apiSection(name, '');
	};

	function selectMainPage() {
		setSelected({
			...selected,
			main: true
		});
	};

	function selectColor(c) {
		setSelected({
			...selected,
			main: false,
			color: c
		});
	};

	function mainPillActive() {
		return selected.main ? 'active' : '';
	}

	function colorPillActive(i) {
		return !selected.main && selected.color === i ? 'active' : '';
	}

	function colorPillName(i) {
		return `Color${nColors < 2 ? '' : ' ' + String.fromCharCode(65 + i)}`
	}

	return (
		<div>
			<div className="d-flex justify-content-between align-items-center flex-wrap mb-3">
				<nav aria-label="breadcrumb">
					<ol className="breadcrumb">
						<li className="breadcrumb-item">
							<a href="#" onClick={home}><i className="fa-solid fa-house"></i> Home</a>
						</li>
						<li className="breadcrumb-item active" aria-current="page">{name}</li>
					</ol>
				</nav>
				<button className="btn btn-outline-secondary btn-sm" disabled={loading} onClick={reload}>
					<i className="fa-solid fa-rotate"></i>
				</button>
			</div>

			<ul className="nav nav-pills mb-3">
				<li className="nav-item">
					<button className={`nav-link ${mainPillActive()}`} onClick={selectMainPage} >{name}</button>
				</li>

				{Array.from({ length: nColors }).map((_, i) => (
					<li className="nav-item" key={"color-pick-pill-" + i}>
						<button className={`nav-link ${colorPillActive(i)}`}
							onClick={() => selectColor(i)}
						>{colorPillName(i)}</button>
					</li>
				))}
			</ul>

			{selected.main && (
				<SectionMain
					section={section}
					info={info}
					apiSection={apiSection}
				/>
			)}
			{!selected.main && (
				<SectionColor
					color={section.color[selected.color]}
					info={info}
					apiSection={apiSection}
				/>
			)}

		</div>
	);
}

function SectionMain({ section, info, apiSection }) {
	return (
		<div>
			main
			<pre>
				{JSON.stringify(section, null, 2)}
			</pre>
		</div>

	);
}

function SectionColor({ color, info, apiSection }) {
	return (
		<div>
			color
			<pre>
				{JSON.stringify(color, null, 2)}
			</pre>
		</div>

	);
}

window.SectionPage = SectionPage;