
const { useState, useEffect, useRef, useCallback, useMemo } = React;


function SectionPage({ name, loading, pixelState, info, apiSection, apiSave, home, saves }) {

	const [section, setSection] = useState({});
	const [selected, setSelected] = useState({
		page: 'main',
		color: 0
	});
	const [nColors, setNColors] = useState(0);

	useEffect(() => {
		setSection(pixelState[name] ?? {});
	}, [pixelState, name])

	useEffect(() => {
		const n = info?.effectInfo[section?.effect]?.ncolorranges ?? 0;
		setNColors(n);

		if (selected.page === 'color' && selected.color >= n) {
			selectMainPage();
		}
	}, [section, info])

	const pageApiSection = useCallback((url) => {
		apiSection(name, url);
	}, [name]);

	const pageColorSection = useCallback((url) => {
		apiSection(name, `color/${selected.color}${url}`);
	}, [name, selected.color]);

	const pageSaveSection = useCallback((save) => {
		apiSave(name, save);
	}, [name]);


	function reload() {
		pageApiSection('');
	};

	function selectMainPage() {
		setSelected({
			...selected,
			page: 'main'
		});
	};

	function selectLoadPage() {
		setSelected({
			...selected,
			page: 'load'
		});
	};

	function selectSavePage() {
		setSelected({
			...selected,
			page: 'save'
		});
	};

	function selectColor(c) {
		setSelected({
			...selected,
			page: 'color',
			color: c
		});
	};

	function mainPillActive() {
		return selected.page === 'main' ? 'active' : '';
	}

	function colorPillActive(i) {
		return selected.page === 'color' && selected.color === i ? 'active' : '';
	}

	function colorPillName(i) {
		return `Color${nColors < 2 ? '' : ' ' + String.fromCharCode(65 + i)}`
	}

	function savePillActive() {
		return selected.page === 'save' ? 'active' : '';
	}

	function loadPillActive() {
		return selected.page === 'load' ? 'active' : '';
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

			<ul className="nav nav-tabs" id="subpageTabs">
				<li className="nav-item">
					<button className={`nav-link ${mainPillActive()}`} onClick={selectMainPage} >Main</button>
				</li>

				{Array.from({ length: nColors }).map((_, i) => (
					<li className="nav-item" key={"color-pick-pill-" + i}>
						<button className={`nav-link ${colorPillActive(i)}`}
							onClick={() => selectColor(i)}
						>{colorPillName(i)}</button>
					</li>
				))}

				<li className="nav-item">
					<button className={`nav-link ${loadPillActive()}`} onClick={selectLoadPage} >Load</button>
				</li>

				<li className="nav-item">
					<button className={`nav-link ${savePillActive()}`} onClick={selectSavePage} >Save</button>
				</li>

			</ul>

			<div className="tab-content p-3">

				{selected.page === 'main' && (
					<SectionMain
						section={section}
						info={info}
						apiSection={pageApiSection}
					/>
				)}
				{selected.page === 'color' && (
					<SectionColor
						color={section.color[selected.color]}
						info={info}
						apiColor={pageColorSection}
					/>
				)}
				{selected.page === 'load' && (
					<SectionLoad
						saves={saves}
						apiSection={pageApiSection}
						loading={loading}
					/>
				)}
				{selected.page === 'save' && (
					<SectionSave
						apiSave={pageSaveSection}
						saves={saves}
						loading={loading}
						name={name}
					/>
				)}
			</div>

		</div>
	);
}

function SectionMain({ section, info, apiSection }) {

	const togglePower = () => {
		if (section?.mode === "on") {
			apiSection("off");
		}
		else {
			apiSection("on");
		}
	}

	const [densityState, setDensityState] = useState(0);

	const apiSectionRef = useRef(apiSection);
	useEffect(() => {
		apiSectionRef.current = apiSection;
	}, [apiSection]);

	function apiSectionStable(url) {
		aapiSectionRef.current(url);
	}

	const densityDebouncer = useRef(new Debouncer(100,
		setDensityState,
		(value) => apiSectionRef.current(`set?density=${value}`)
	));

	const activeEffectPill = useRef(null);

	useEffect(() => {
		setDensityState(section?.density ?? 1);

		if (activeEffectPill.current) {
			activeEffectPill.current.scrollIntoView({ behavior: "smooth", inline: "center" });
		}
	}, [section]);

	function effectPillActive(name) {
		return section?.effect === name ? 'active' : '';
	}

	function setEffect(value) {
		apiSection(`set?effect=${value}`);
	}

	return (
		<div>
			<div className="d-flex justify-content-around align-items-center flex-wrap mb-3">
				<button
					className={`btn btn-outline-${section?.mode === "on" ? "success" : "danger"} btn-md`}
					onClick={togglePower}
					style={{ width: "100px", height: "100px", borderRadius: "50%" }}
				>
					<i className="fa-solid fa-power-off fa-2x"></i>
				</button>
			</div>



			<div className="d-flex align-items-center ">
				<input
					type="range"
					className="form-range"
					min="1"
					max="10"
					value={densityState}
					onChange={(e) => densityDebouncer.current.set(Number(e.target.value))}
				/>
			</div>

			<div className="d-flex justify-content-between w-100 mb-4">

				{Array.from({ length: 11 }).map((_, i) => (
					<i
						key={`bulb-${i}`}
						className="fa-solid fa-lightbulb"
						style={{ color: i % densityState == 0 ? 'gold' : 'darkgray' }}></i>
				))}

			</div>

			<div className="overflow-auto scrollable-pills">
				<ul className="nav nav-pills flex-nowrap">

					{info?.effects?.map((ename) => (
						<li key={`effect-pill-${ename}`}
							className="nav-item">
							<button className={`nav-link ${effectPillActive(ename)}`}
								onClick={() => setEffect(ename)}
								ref={section?.effect === ename ? activeEffectPill : null}
							>{ename}</button>
						</li>
					))}
				</ul>
			</div>
		</div>

	);
}


function toRgb(value) {
	// Remove leading #
	value = value.replace(/^#/, '');

	if (value.length !== 6) {
		console.log(`weird color value ${value}`)
		return;
	}

	const r = parseInt(value.slice(0, 2), 16);
	const g = parseInt(value.slice(2, 4), 16);
	const b = parseInt(value.slice(4, 6), 16);

	return `r=${r}&g=${g}&b=${b}`;

}

function SectionColor({ color, info, apiColor }) {
	const apiColorStableRef = useRef(apiColor);
	useEffect(() => apiColorStableRef.current = apiColor, [apiColor])

	const apiColorStable = (url) => apiColorStableRef.current(url);

	const [fromColorState, setFromColorState] = useState('#000000');
	const fromColorDebouncer = useRef(new Debouncer(100, setFromColorState,
		(value) => apiColorStable(`/from?${toRgb(value)}`)
	));

	const [toColorState, setToColorState] = useState('#000000');
	const toColorDebouncer = useRef(new Debouncer(100, setToColorState,
		(value) => apiColorStable(`/to?${toRgb(value)}`)
	));

	const [biasState, setBiasState] = useState(500);
	const biasDebouncer = useRef(new Debouncer(100, setBiasState,
		(value) => apiColorStable(`?bias=${value / 10000}`)
	));

	const CYCLE_SPEED_MIN = 10;
	const CYCLE_SPEED_MAX = 1000 * 60 * 10;

	const FRAME_RATE_MIN = 50;
	const FRAME_RATE_MAX = 1000 * 60;

	const [cycleSpeedState, setCycleSpeedState] = useState(color?.cycleSpeed ?? 500);
	const [frameRateState, setFrameRateState] = useState(color?.frameRate ?? 500);

	const activeInterpolationPill = useRef(null);

	function interpolationPillActive(name) {
		return color?.interpolation === name ? 'active' : '';
	}


	useEffect(() => {
		setFromColorState(color?.from ?? '#000000');
		setToColorState(color?.to ?? '#000000');
		setBiasState(color?.bias * 10000 ?? 500);
		setCycleSpeedState(color?.cycleSpeed ?? CYCLE_SPEED_MIN);
		setFrameRateState(color?.frameDuration ?? FRAME_RATE_MIN);
		if (activeInterpolationPill.current) {
			activeInterpolationPill.current.scrollIntoView({ behavior: "smooth", inline: "center" });
		}

	}, [color])

	return (
		<div>
			<div className="row mb-3">
				<label className="col-sm-3 col-form-label">From/To</label>
				<div className="col-sm-9">
					<ColorButton color={fromColorState} onChange={v => fromColorDebouncer.current.set(v)} />
					<ColorButton color={toColorState} onChange={v => toColorDebouncer.current.set(v)} />
				</div>
			</div>

			<div className="row mb-3">
				<label className="col-sm-3 col-form-label">Transition</label>
				<div className="col-sm-9">



					<div className="overflow-auto scrollable-pills">
						<ul className="nav nav-pills flex-nowrap">
							{info?.interpolations?.map((iname) => (
								<li key={`interpolation-pill-${iname}`}
									className="nav-item">
									<button className={`nav-link ${interpolationPillActive(iname)}`}
										onClick={() => apiColorStable(`?interpolation=${iname}`)}
										ref={color?.interpolation === iname ? activeInterpolationPill : null}
									>{iname}</button>
								</li>
							))}
						</ul>
					</div>


				</div>
			</div>

			<div className="row mb-3">
				<label className="col-sm-3 col-form-label">Seamless</label>
				<div className="col-sm-9">
					<input
						type="checkbox"
						className="form-check-input"
						checked={color.seamless}
						onChange={e => {
							const value = e.target.checked;
							apiColorStable(`?seamless=${value}`)
						}}
					/>
				</div>
			</div>

			<div className="row mb-3">
				<label className="col-sm-3 col-form-label">Bias</label>
				<div className="col-sm-9">
					<input
						type="range"
						className="form-range"
						min="1"
						max="9999"
						value={biasState}
						onChange={e => biasDebouncer.current.set(parseFloat(e.target.value))}
					/>
				</div>
			</div>

			<hr />

			<div className="row ">
				<label className="col-sm-3 col-form-label">Animating</label>
				<div className="col-sm-9">
					<input
						type="checkbox"
						className="form-check-input"
						checked={color.animating}
						onChange={e => {
							const value = e.target.checked;
							apiColorStable(`?animating=${value}`)
						}}
					/>
				</div>
			</div>

			<DurationSlider
				label={'Cycle speed'}
				value={cycleSpeedState}
				setValue={setCycleSpeedState}
				min={CYCLE_SPEED_MIN}
				max={CYCLE_SPEED_MAX}
				api={(value) => apiColorStable(`?cycleSpeed=${value}`)}
			/>

			<DurationSlider
				label={'Frame rate'}
				value={frameRateState}
				setValue={setFrameRateState}
				min={FRAME_RATE_MIN}
				max={FRAME_RATE_MAX}
				api={(value) => apiColorStable(`?frameRate=${value}`)}
			/>
		</div>

	);
}

function SectionLoad({ saves, apiSection, loading }) {

	const [selected, setSelected] = useState(undefined);

	useEffect(() => {
		setSelected(undefined);
	}, [saves])

	const sortedSaves = useMemo(
		() => (saves && [...saves].sort((a, b) => a.name.localeCompare(b.name))) ?? [],
		[saves]
	);

	function load(name) {
		if (selected) {
			apiSection(`load?name=${encodeURIComponent(name)}`)
		}
	}

	function rowClass(save) {
		return selected === save ? 'active' : '';
	}

	return (<div className="container py-3">
		<div className="list-group overflow-auto" style={{ 'maxHeight': '70vh' }}>
			{sortedSaves.map((save) =>
				<button className={`list-group-item list-group-item-action d-flex justify-content-between ${rowClass(save)}`}
					key={save.name}
					onClick={(event) => {setSelected(save); load(save.name);}}
					type="button"
				>
					<span>{save.name}</span>
					<small className="text-muted">{save.date}</small>
				</button>

			)}
		</div>
	</div>
	);

}

function SectionSave({ apiSave, saves, loading, name }) {
	const sortedSaves = useMemo(
		() => (saves && [...saves].sort((a, b) => a.name.localeCompare(b.name))) ?? [],
		[saves]
	);

	const [saveName, setSaveName] = useState('');

	return (

		<div className="container py-3">
			<label htmlFor="saveName" className="form-label">Save current settings for {name} as</label>

			<div className="input-group mb-3">
				<input
					list="existingSaves"
					id="saveName"
					className="form-control"
					placeholder="name..."
					value={saveName}
					onChange={(e) => setSaveName(e.target.value)}
				/>

				<button
					className="btn btn-primary"
					type="button"
					onClick={() => apiSave(saveName)}
					disabled={!saveName || loading}
				>
					Save
				</button>
			</div>

			<datalist id="existingSaves">
				{sortedSaves.map((save) => (
					<option key={save.name} value={save.name}>
						{save.name}
					</option>
				))}
			</datalist>
		</div>

	);
}

window.SectionPage = SectionPage;